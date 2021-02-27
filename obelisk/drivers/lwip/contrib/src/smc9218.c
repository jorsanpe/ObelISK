/*
 * $FILE: smsc9218.c
 *
 * SMSC LAN9218 driver. The current implementation of the eth_rx funcion
 * can cause the system to infringe real-time constraints. This function is
 * called in the context of an interrupt handler. As the number of packets
 * the incoming message has is unpredictable (despite bounded) the interrupt
 * latency could grow too much.
 *
 * TODO: DMA preparation and launch on thread context. Use interrupt handler
 * only to notify the arrival of data to the device.
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <autoconf.h>
#include <smc9218.h>
#include <lwip_log.h>
#include <arch/delay.h>
#include <arch/sys_arch.h>
#include <arch/clock.h>
#include <transport.h>


#define printf(...)

static inline uint32_t reg_read(uint32_t addr)
{
   //return *(volatile uint32_t*) addr;
   return (unsigned int) (*(volatile unsigned int *) addr);
}

static inline void reg_write(uint32_t addr, uint32_t val)
{
   *(volatile unsigned int *) addr = val;
}

uint32_t pkt_data_pull(uint32_t addr) __attribute__ ((weak, alias ("reg_read")));
void pkt_data_push(uint32_t addr, uint32_t val) __attribute__ ((weak, alias ("reg_write")));

uint32_t smc911x_get_mac_csr(uint8_t reg)
{
   while (reg_read(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY)
      ;

   reg_write(MAC_CSR_CMD, MAC_CSR_CMD_CSR_BUSY | MAC_CSR_CMD_R_NOT_W | reg);

   while (reg_read(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY)
      ;

   return reg_read(MAC_CSR_DATA);
}

void smc911x_set_mac_csr(uint8_t reg, uint32_t data)
{
   while (reg_read(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY)
      ;

   reg_write(MAC_CSR_DATA, data);
   reg_write(MAC_CSR_CMD, MAC_CSR_CMD_CSR_BUSY | reg);

   while (reg_read(MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY)
      ;
}

static int32_t smx911x_handle_mac_address(uint8_t *m)
{
   uint32_t addrh, addrl;

   /* if the environment has a valid mac address then use it */
   if ((m[0] | m[1] | m[2] | m[3] | m[4] | m[5])) {
      addrl = m[0] | m[1] << 8 | m[2] << 16 | m[3] << 24;
      addrh = m[4] | m[5] << 8;
      smc911x_set_mac_csr(ADDRH, addrh);
      smc911x_set_mac_csr(ADDRL, addrl);
   } else {
      /* if not, try to get one from the eeprom */
      addrh = smc911x_get_mac_csr(ADDRH);
      addrl = smc911x_get_mac_csr(ADDRL);

      m[0] = (addrl) & 0xff;
      m[1] = (addrl >> 8) & 0xff;
      m[2] = (addrl >> 16) & 0xff;
      m[3] = (addrl >> 24) & 0xff;
      m[4] = (addrh) & 0xff;
      m[5] = (addrh >> 8) & 0xff;

      /* we get 0xff when there is no eeprom connected */
      if ((m[0] & m[1] & m[2] & m[3] & m[4] & m[5]) == 0xff) {
         printf(DRIVERNAME ": no valid mac address in environment "
               "and no eeprom found\n");
         return ESMC_INIT;
      }
   }

   printf(DRIVERNAME ": MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
         m[0], m[1], m[2], m[3], m[4], m[5]);

   return ESMC_OK;
}

static int32_t smc911x_miiphy_read(uint8_t phy, uint8_t reg, uint16_t *val)
{
   while (smc911x_get_mac_csr(MII_ACC) & MII_ACC_MII_BUSY)
      ;

   smc911x_set_mac_csr(MII_ACC, phy << 11 | reg << 6 | MII_ACC_MII_BUSY);

   while (smc911x_get_mac_csr(MII_ACC) & MII_ACC_MII_BUSY)
      ;

   *val = smc911x_get_mac_csr(MII_DATA);

   return 0;
}

static int32_t smc911x_miiphy_write(uint8_t phy, uint8_t reg, uint16_t val)
{
   while (smc911x_get_mac_csr(MII_ACC) & MII_ACC_MII_BUSY)
      ;

   smc911x_set_mac_csr(MII_DATA, val);
   smc911x_set_mac_csr(MII_ACC, phy << 11 | reg << 6 | MII_ACC_MII_BUSY
         | MII_ACC_MII_WRITE);

   while (smc911x_get_mac_csr(MII_ACC) & MII_ACC_MII_BUSY)
      ;
   return 0;
}

static int32_t smc911x_phy_reset(void)
{
   uint32_t reg;

   reg = reg_read(PMT_CTRL);
   reg &= ~0xfffff030;
   reg |= PMT_CTRL_PHY_RST;
   reg_write(PMT_CTRL, reg);

   mdelay(100);

   return 0;
}

static void smc911x_phy_configure(void)
{
   int32_t timeout;
   uint16_t status;

   smc911x_phy_reset();

   smc911x_miiphy_write(1, PHY_BMCR, PHY_BMCR_RESET);
   mdelay(1);
   smc911x_miiphy_write(1, PHY_ANAR, 0x01e1);
   smc911x_miiphy_write(1, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);

   timeout = 5000;
   do {
      mdelay(1);
      if ((timeout--) == 0)
         goto err_out;

      if (smc911x_miiphy_read(1, PHY_BMSR, &status) != 0)
         goto err_out;
   } while (!(status & PHY_BMSR_LS));

   printf(DRIVERNAME ": phy initialized\n");

   return;

   err_out: printf(DRIVERNAME ": autonegotiation timed out\n");
}

static void smc911x_reset(void)
{
   int32_t timeout;

   // Take out of PM setting first
   if (reg_read(PMT_CTRL) & PMT_CTRL_READY) {
      // Write to the bytetest will take out of powerdown
      reg_write(BYTE_TEST, 0x0);

      timeout = 10;

      while (timeout-- && !(reg_read(PMT_CTRL) & PMT_CTRL_READY)) {
         udelay(10);
      }
      if (!timeout) {
         printf(DRIVERNAME ": timeout waiting for PM restore\n");
         return;
      }
   }

   /* Disable interrupts */
   reg_write(INT_EN, 0);

   reg_write(HW_CFG, HW_CFG_SRST);

   timeout = 1000;
   while (timeout-- && reg_read(E2P_CMD) & E2P_CMD_EPC_BUSY) {
      udelay(10);
   }

   if (!timeout) {
      printf(DRIVERNAME ": reset timeout\n");
      return;
   }

   /* Reset the FIFO level and flow control settings */
   smc911x_set_mac_csr(FLOW, FLOW_FCPT | FLOW_FCEN);
   reg_write(AFC_CFG, 0x0050287F);

   /* Set to LED outputs */
   reg_write(GPIO_CFG, 0x70070000);
}

static void smc911x_enable(void)
{
   uint32_t tmpreg;

   // Enable TX
   reg_write(HW_CFG, 8 << 16 | HW_CFG_SF);
   reg_write(GPT_CFG, GPT_CFG_TIMER_EN | 10000);
   reg_write(TX_CFG, TX_CFG_TX_ON);

   // Enable chip rx interrupt
   tmpreg = reg_read(INT_CFG);
   tmpreg |= INT_CFG_IRQ_EN;
   reg_write(INT_CFG, tmpreg);// | INT_CFG_IRQ_POL | INT_CFG_IRQ_TYPE);

   // Clear any pending interrupt
   reg_write(INT_STS, INT_STS_RSFL);

   // Enable reception interrupt
   //tmpreg = reg_read(INT_EN);
   //tmpreg = INT_EN_RSFL_EN;
   reg_write(INT_EN, INT_EN_RSFL_EN);

   // No padding to start of packets
   reg_write(RX_CFG, 0);

   smc911x_set_mac_csr(MAC_CR, MAC_CR_TXEN | MAC_CR_RXEN | MAC_CR_HBDIS);
}

/*
 * dev_handle_int()
 *
 *    The LAN9218 interrupt handler
 */
void smc911x_handle_int(void)
{
   uint32_t mask, stat;

   // Read interrupt mask and disable interrupts
   mask = reg_read(INT_EN);
   reg_write(INT_EN, 0);
   stat = reg_read(INT_STS) & mask;

   // The interrupt has been caused by the reception FIFO
   if (stat & INT_STS_RSFL) {
      // Acknowledge the rx interrupt bit
      reg_write(INT_STS, INT_STS_RSFL);

      // New message available on the LAN9218 FIFO
      if (eth_rx() != ESMC_OK) {
         // Handle bad return from reading packets

         // Acknowledge interrupt
         bfin_write_PINT2_IRQ(PE6);

         // Re-enable interrupts
         reg_write(INT_EN, mask);
         return;// ESMC_INT_ERROR;
      }

      // Acknowledge interrupt
      bfin_write_PINT2_IRQ(PE6);

      // Re-enable interrupts
      reg_write(INT_EN, mask);
      return;// ESMC_INT_RX;
   }

   // Acknowledge interrupt
   bfin_write_PINT2_IRQ(PE6);

   // Re-enable interrupts
   reg_write(INT_EN, mask);

   // At this point no other interrupt cause is handled
   return;// ESMC_INT_UNKNOWN;
}

/*
 * eth_init()
 *
 *    Ethernet initialization
 *
 *    @mac    -> MAC address
 *    @return -> ESMC_OK if success, or ESMC_INIT if an error occurred
 */
int32_t eth_init(uint8_t *mac)
{
   uint32_t val, i;
   uint32_t tmout, tmp;

   printf(DRIVERNAME ": initializing\n");

   // Reset the Chip
   reg_write(HW_CFG, HW_CFG_SRST);
   tmout = 100000UL;
   do {
      usleep(10);
      tmp = reg_read(HW_CFG);
      tmout--;
   } while ((tmout-- > 0UL) && (tmp & HW_CFG_SRST));

   //Failed to initialize LAN9218
   if (tmp & HW_CFG_SRST) {
      return ESMC_INIT;
   }

   val = reg_read(BYTE_TEST);
   if (val != 0x87654321) {
      printf(DRIVERNAME ": Invalid chip endian 0x%08lx\n", val);
      return ESMC_INIT;
   }

   val = reg_read(ID_REV) >> 16;
   for (i = 0; chip_ids[i].id != 0; i++) {
      if (chip_ids[i].id == val) {
         break;
      }
   }
   if (!chip_ids[i].id) {
      printf(DRIVERNAME ": Unknown chip ID %04lx\n", val);
      return ESMC_INIT;
   }

   printf(DRIVERNAME ": detected %s controller\n", chip_ids[i].name);

   smc911x_reset();

   /* Configure the PHY, initialize the link state */
   smc911x_phy_configure();

   if (smx911x_handle_mac_address(mac) == ESMC_INIT) {
      return ESMC_INIT;
   }

   /* Turn on Tx + Rx */
   smc911x_enable();

   return ESMC_OK;
}

/*
 * eth_add_packet()
 */
/*void eth_add_packet(void *packet, int32_t length)
{
   // This code is run inside an interrupt handler so there cannot be race conditions
   eth_ring.ready[eth_ring.freerdy].index = eth_ring.dmaidx;
   eth_ring.ready[eth_ring.freerdy].flags = 0;
   eth_ring.src_dpool[eth_ring.dmaidx] = (struct dma_descriptor) {
      .start_addr = (void *)packet,
      .config = WDSIZE_16 | DMAEN,
      .x_count = length,
      .x_modify = 2,
   };
   eth_ring.dst_dpool[eth_ring.dmaidx] = (struct dma_descriptor) {
      .start_addr = (void *)TX_DATA_FIFO,
      .config = DI_EN | WDSIZE_16 | WNR | DMAEN,
      .x_count = length,
      .x_modify = 0,
   };
   eth_ring.dmaidx = (eth_ring.dmaidx + 1) & ~LWIP_DPOOL_SIZE;
   eth_ring.freerdy = (eth_ring.freerdy + 1) & ~LWIP_DPOOL_SIZE;
}*/


/*
 * eth_send()
 *
 *    Send a packet over Ethernet
 */
int32_t eth_send(volatile void *packet, int32_t length)
{
   uint32_t *data = (uint32_t *) ((uint32_t)packet & ~0x3); // Correct alignment
   uint32_t tmplen, status, count, space;
   static uint32_t seqcnt = 0;

   // Wait until space available
   count = 0;
   tmplen = (length + 3) & (~0x03);
   while (1) {
      space = reg_read(TX_FIFO_INF) & TX_FIFO_INF_TDFREE;
      if (tmplen <= space) {
         break;
      }
      usleep(10);
      if (count++ > 100) {
         // Timeout while waiting for space
         LWIPPRINT("Failed to send packet. Timeout while waiting for space");
         return ESMC_SEND;
      }
   }

   tmplen = (length + 3 + ((uint32_t)packet & 0x3)) >> 2;

   reg_write(TX_DATA_FIFO, TX_CMD_A_INT_FIRST_SEG | TX_CMD_A_INT_LAST_SEG | length | (((uint32_t)packet & 0x3) << 16));
   reg_write(TX_DATA_FIFO, length | (seqcnt<<16));
   seqcnt ++;

   // Write fifo
   while (tmplen--) {
      pkt_data_push(TX_DATA_FIFO, *data++);
   }

   /* wait for transmission */
   //while (!((reg_read(TX_FIFO_INF) & TX_FIFO_INF_TSUSED) >> 16));

   /* get status. Ignore 'no carrier' error, it has no meaning for
    * full duplex operation
    */
   status = reg_read(TX_STATUS_FIFO) & (TX_STS_LOC | TX_STS_LATE_COLL
         | TX_STS_MANY_COLL | TX_STS_MANY_DEFER | TX_STS_UNDERRUN);

   if (!status) {
      //LWIPPRINT("Packet Sent");
      return ESMC_OK;
   }

   LWIPPRINT("Failed to send packet");

   printf(DRIVERNAME ": failed to send packet: %s%s%s%s%s\n",
         status & TX_STS_LOC ? "TX_STS_LOC " : "",
         status & TX_STS_LATE_COLL ? "TX_STS_LATE_COLL " : "",
         status & TX_STS_MANY_COLL ? "TX_STS_MANY_COLL " : "",
         status & TX_STS_MANY_DEFER ? "TX_STS_MANY_DEFER " : "",
         status & TX_STS_UNDERRUN ? "TX_STS_UNDERRUN" : "");

   return ESMC_SEND;
}

/*
 * eth_halt()
 *
 *    Halt device
 */
void eth_halt(void)
{
   smc911x_reset();
}

/*
 * eth_rx()
 *
 *    This function is called after the LAN9218 interrupt is asserted
 *
 *    @p      -> pbuf chain read in case it returns successfully
 *    @return -> ESMC_RCV if an error ocurred or ESMC_OK if all was ok
 */
int32_t eth_rx(void)
{
   uint32_t pktlen, prevdesc, status;
   struct pbuf *p, *q;
   struct tring_request *request;
   struct dma_descriptor *src, *dst;

   // Check if the interrupt has actually been caused by the reception of a new frame
   //if ((reg_read(RX_FIFO_INF) & RX_FIFO_INF_RXSUSED) >> 16) {
   status = reg_read(RX_STATUS_FIFO);

   // Check packet status
   if (status & RX_STS_ES) {
      LWIPPRINT("Dropped bad packet");
      return ESMC_RCV;
   }

   // Received packet length
   pktlen = (status & RX_STS_PKT_LEN) >> 16;
   pktlen &= 0x07ff;

   reg_write(RX_CFG, 0);

   // We allocate a pbuf chain of pbufs from the pool
   p = pbuf_alloc(PBUF_RAW, pktlen + 2 + 3, PBUF_POOL);

   // Not enough memory
   if (p == NULL) {
      LWIPPRINT("Not Enough memory for packet reception");
      return ESMC_RCV;
   }

   // Create a new request, chaining DMA descriptors
   request = mpalloc(eth_request_pool);
   q = request->pbuf = p;
   src = request->src = mpalloc(eth_dma_desc_pool);
   dst = request->dst = mpalloc(eth_dma_desc_pool);
   request->flags = TRING_CHAIN_RX;
   while (dst != NULL && src != NULL) {
      src->start_addr = (void *)RX_DATA_FIFO;
      src->config = DMAFLOW_LARGE | NDSIZE_9 | WDSIZE_32 | DMAEN;
      src->x_count = q->len >> 2;
      src->x_modify = 0;

      dst->start_addr = q->payload;
      dst->config = DMAFLOW_LARGE | NDSIZE_9 | WDSIZE_32 | WNR | DMAEN;
      dst->x_count = q->len >> 2;
      dst->x_modify = 4;

      q = q->next;

      if (q == NULL) {  // No more pbufs
         break;
      }
      dst->next = mpalloc(eth_dma_desc_pool);
      dst = dst->next;
      src->next = mpalloc(eth_dma_desc_pool);
      src = src->next;
   }

   // ¿Not enough memory? Free what we have allocated
   if (src == NULL || dst == NULL) {
      clear_request(request);
      LWIPPRINT("Not Enough memory for packet reception");
      return ESMC_RCV;
   }

   // Last descriptor must stop DMA chain
   src->config = WDSIZE_32 | DMAEN;
   dst->config = DI_EN | WDSIZE_32 | WNR | DMAEN;

   // Activate or enqueue request
   if (!eth_active_request) {
      eth_active_request = request;
      //START_DMA_CHAIN(request->src, request->dst);
      mdma_launch_transfer(eth_dma_channel, request->src, request->dst);
   } else {
      request->tring_list.this = request;
      request->tring_list.prev = request->tring_list.next = NULL;
      queue_insert(&eth_request_queue, &request->tring_list);
   }

   // Read the FIFO from the LAN9218
   /*while (tmplen--) {
      *data++ = pkt_data_pull(RX_DATA_FIFO);
   }*/
   //LWIPPRINT("Packet Received");

   return ESMC_OK;
}





