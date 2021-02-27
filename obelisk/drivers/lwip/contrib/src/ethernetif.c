/*
 * $FILE: ethernetif.c
 *
 * Ethernet Interface Implementation
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 *
 * $LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/sys.h>
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <netif/etharp.h>
#include <netif/ppp_oe.h>
#include <arch/sys_arch.h>

#include <mpool.h>
#include <sched.h>
#include <tlsf.h>
#include <transport.h>

#include <stdlib.h>
#include <smc9218.h>
#include <autoconf.h>
#include <processor.h>
#include <linkage.h>
#include <ethernetif.h>

struct ethernetif {
   struct netif *netif;
};

static struct ethernetif *ethernetif;
static ID etharp_timer_thread_id;

__tring struct mpool *eth_dma_desc_pool;
__tring struct mpool *eth_request_pool;
__tring queue_t eth_request_queue;
__tring int32_t eth_dma_channel;
__tring struct tring_request *eth_active_request;
//__tring struct transport_ring eth_ring;

static void etharp_timer_thread(void)
{
  while (1) {
    etharp_tmr();
    tslp_tsk_sys(5000);
  }
}

void etharp_timer_init(void)
{
   T_CTSK pk_ctsk;

   // Assign values to the task creation packet
   pk_ctsk.task = (void (*)(void))etharp_timer_thread;
   pk_ctsk.stksz = 512;
   pk_ctsk.itskpri = CONFIG_ETHIF_PRIO;
   pk_ctsk.exinf = (void *)TSK_KERNEL;
   pk_ctsk.stk = malloc_ex(pk_ctsk.stksz, system_pool);;

   // Create task with automatic ID assignment
   etharp_timer_thread_id = acre_tsk_sys(&pk_ctsk);
   act_tsk_sys(etharp_timer_thread_id);
}

/*
 * tring_int_handler()
 *
 *    Triggered after a DMA descriptor chain has completed
 */
void tring_int_handler(void)
{
   struct pbuf *p, *q;
   //SYS_ARCH_DECL_PROTECT(flags);

   // Acknowledge interrupt
   mdma_ack_int(eth_dma_channel);
   /*bfin_write_MDMA_S3_CONFIG(0);
   bfin_write_MDMA_D3_CONFIG(0);
   bfin_write_MDMA_S3_IRQ_STATUS(DMA_DONE);
   bfin_write_MDMA_D3_IRQ_STATUS(DMA_DONE);*/

   // If receive operation, queue received packets
   if (eth_active_request->flags & TRING_CHAIN_RX) {
      p = (struct pbuf *)eth_active_request->pbuf;
      for (q = p; q != NULL; q = q->next) {
         // Enqueue the received packets. The tcpip_thread will later drain this queue
         tcpip_input(q, ethernetif->netif);
      }
      LWIPPRINT("Packet Received");
   }

   // Free request resources
   clear_request(eth_active_request);

   // Check if there are ready descriptors to add to the chain
   if (queue_get_head(&eth_request_queue)) {
      eth_active_request = queue_extract(&eth_request_queue);
      mdma_launch_transfer(eth_dma_channel, eth_active_request->src, eth_active_request->dst);
      //START_DMA_CHAIN(eth_active_request->src, eth_active_request->dst);
   } else {
      eth_active_request = NULL;
   }
}

/*
 * low_level_init()
 *
 *    In this function, the hardware is initialized. Called from
 *    ethernetif_init(). @param netif the already initialized lwip
 *    network interface structure for this ethernetif
 */
__init static void low_level_init(struct netif *netif)
{
   u32_t tmpreg;
   T_DINH pk_dinh;

   // Device capabilities
   netif->mtu = MTU - 18;                 // MTU without ethernet header and cr
   netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

   // set MAC hardware address length
   netif->hwaddr_len = ETHARP_HWADDR_LEN;

   // set port H MUX to configure PH8-PH11 as 1st Function (MUX = 00)
   // (bits 16-27 == 0) - Address signals A4-A9
   bfin_write_PORTH_FER(0x3F00);
   bfin_write_PORTH_MUX(0x0);
   bfin_write_PORTH_DIR_SET(0x3F00);

   // SMSC LAN9218 device initialization, basically set MAC address
   eth_init(netif->hwaddr);

   // Port E6 enable as input
   tmpreg = bfin_read_PORTE_INEN() | PE6;
   bfin_write_PORTE_INEN(tmpreg);

   // Port E6 enable as input
   tmpreg = bfin_read_PORTE_DIR_CLEAR() | PE6;
   bfin_write_PORTE_DIR_CLEAR(tmpreg);

   // Port E6 active low
   tmpreg = bfin_read_PINT2_INVERT_SET() | PE6;
   bfin_write_PINT2_INVERT_SET(tmpreg);

   // Port E mask interrupt (enable PE6 interrupt)
   tmpreg = bfin_read_PINT2_MASK_SET() | PE6;
   bfin_write_PINT2_MASK_SET(tmpreg);

   // Pin to interrupt assignment (PE6 -> PINT2)
   //tmpreg = bfin_read_PINT2_ASSIGN();
   //tmpreg = (tmpreg & 0xffff0000) | B0MAP_PEL;
   bfin_write_PINT2_ASSIGN(0x00000202);

   // Pin interrupt latch (clear possible pending PINT2 interrupts)
   tmpreg = bfin_read_PINT2_LATCH();
   bfin_write_PINT2_IRQ(tmpreg);

   // System interrupt assignment register (PINT2 -> EVT12)
   tmpreg = (bfin_read_SIC_IAR11() & 0xf0ffffff) | 0x05000000;
   bfin_write_SIC_IAR11(tmpreg);

   // Sistem interrupt mask register (enable PINT2 interrupt)
   tmpreg = bfin_read_SIC_IMASK2() | (1 << 30);
   bfin_write_SIC_IMASK2(tmpreg);

   // Initialize device interrupt handler
   pk_dinh.inthdr = smc911x_handle_int;
   def_inh_sys(12, &pk_dinh);

   // Initialize transport ring interrupt handler
   // System interrupt assignment register (MDMA Stream 3 -> EVT13)
   tmpreg = (bfin_read_SIC_IAR6() & 0xfffff0ff) | 0x00000600;
   bfin_write_SIC_IAR6(tmpreg);
   tmpreg = bfin_read_SIC_IMASK1() | (1 << 18);
   bfin_write_SIC_IMASK1(tmpreg);
   pk_dinh.inthdr = tring_int_handler;
   def_inh_sys(13, &pk_dinh);
}

/**
 * This function does the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
   struct pbuf *q;

   //initiate transfer();

#if ETH_PAD_SIZE
   pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

   for (q = p; q != NULL; q = q->next) {
      // Send the data from the pbuf to the interface, one pbuf at a
      // time. The size of the data in each pbuf is kept in the ->len
      // variable
      eth_send(q->payload, q->len);
   }
   //pbuf_free(p);

#if ETH_PAD_SIZE
   pbuf_header(p, ETH_PAD_SIZE); // reclaim the padding word
#endif

   LINK_STATS_INC(link.xmit);

   return ERR_OK;
}

/*
 * ethernetif_init()
 *
 * Ethernet interface initialization procedure. This function is passed
 * as a parameter to netif_add()
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
__init err_t ethernetif_init(struct netif *netif)
{
   uint32_t i;

   ethernetif = sys_malloc(sizeof(ethernetif));
   ethernetif->netif = netif;

   LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
   /* Initialize interface hostname */
   netif->hostname = "obelisk";
#endif /* LWIP_NETIF_HOSTNAME */

   // Initialize the snmp variables and counters inside the struct netif
   NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_100MBPS);

   netif->state = NULL;
   netif->name[0] = IFNAME0;
   netif->name[1] = IFNAME1;
   netif->output = etharp_output;
   netif->linkoutput = low_level_output;

   // Initialize the hardware
   low_level_init(netif);

   // Initialize the transport mechanism
   // Pool for the DMA descriptors
   eth_dma_desc_pool = sys_malloc(POOL_SIZE(DMA_POOL_SIZE, sizeof(struct dma_descriptor)));
   if (!eth_dma_desc_pool) {
      kernel_panic();
   }
   init_pool(eth_dma_desc_pool, sizeof(struct dma_descriptor), DMA_POOL_SIZE);

   // Pool for the requests
   eth_request_pool = sys_malloc(POOL_SIZE(LWIP_DPOOL_SIZE, sizeof(struct tring_request)));
   if (!eth_request_pool) {
      kernel_panic();
   }
   init_pool(eth_request_pool, sizeof(struct tring_request), LWIP_DPOOL_SIZE);

   // This may need further processing. However, this should never end in kernel panic
   // as this initialization code is not preceeded by any dma channel allocation.
   eth_dma_channel = mdma_allocate_channel();
   if (eth_dma_channel < 0) {
      kernel_panic();
   }

   return ERR_OK;
}

