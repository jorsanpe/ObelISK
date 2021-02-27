
extern char SetFail;
#include "LedsDevice.h"

#include <p30fxxxx.h>
#include "can.h"
#include "uart.h"

#include "SystemInfo.h"
#include "KertrolTinyMWCodeDelegationService.h"
#include "KertrolTinyMWNetworkService.h"
#include "KertrolTinyMWSensoryService.h"
#include "KertrolTinyMWActuatorService.h"
#include "KertrolTinyMWDataManagerService.h"
#include "KertrolTinyMWAdministrationAndMonitoringService.h"


////////////////////////////
/// Output CAN Buffer /////
//////////////////////////

/// Variables of the buffer ///
struct CanMessage OutputCANBuffer[CAN_BUFFER_SIZE]; //buffer where the CAN messages are saved before bieng sent
unsigned char CANBufferEntryPointer; //Pointer to the entry point of the buffer
unsigned char CANBufferExitPointer; //Pointer to the exit point of the buffer
unsigned char CANBufferlength; //Keeps the number of CAN messages saved currently in the buffer

////
/// Initializes the output CAN buffer
void OutputCANBufferInit()
{
	unsigned char i;
	
	//Inicializacion del buffer de mensajes CAN de salida
	CANBufferEntryPointer = 0;
	CANBufferExitPointer = 0;
	CANBufferlength = 0;
	
	for(i=0; i<CAN_BUFFER_SIZE; i++)
	{
		OutputCANBuffer[i].Id = 0x00000000;
		OutputCANBuffer[i].DataLength = 0x00;
		OutputCANBuffer[i].Data[0] = 0x00;
		OutputCANBuffer[i].Data[1] = 0x00;
		OutputCANBuffer[i].Data[2] = 0x00;
		OutputCANBuffer[i].Data[3] = 0x00;
		OutputCANBuffer[i].Data[4] = 0x00;
		OutputCANBuffer[i].Data[5] = 0x00;
		OutputCANBuffer[i].Data[6] = 0x00;
		OutputCANBuffer[i].Data[7] = 0x00;
	}
}	
////
/// Enqueues the fields of one CAN message to the circular buffer. Returns -1 if there is no space available in the buffer
char QueueFieldsCanMessageOutputCANBuffer(unsigned long Id, unsigned char DataLength, unsigned char Data[8])
{
	if (CANBufferlength >= CAN_BUFFER_SIZE)//The buffer is full
	{
		return (-1);
	}
	
	//Queuing the CAN message
	OutputCANBuffer[CANBufferEntryPointer].Id = Id;
	OutputCANBuffer[CANBufferEntryPointer].DataLength = DataLength;
	OutputCANBuffer[CANBufferEntryPointer].Data[0] = Data[0];
	OutputCANBuffer[CANBufferEntryPointer].Data[1] = Data[1];
	OutputCANBuffer[CANBufferEntryPointer].Data[2] = Data[2];
	OutputCANBuffer[CANBufferEntryPointer].Data[3] = Data[3];
	OutputCANBuffer[CANBufferEntryPointer].Data[4] = Data[4];
	OutputCANBuffer[CANBufferEntryPointer].Data[5] = Data[5];
	OutputCANBuffer[CANBufferEntryPointer].Data[6] = Data[6];
	OutputCANBuffer[CANBufferEntryPointer].Data[7] = Data[7];
	
	//Increasing the entry pointer 
	CANBufferEntryPointer++;
	//Increasing the buffer length
	CANBufferlength++;

	//Check the pointer doesn't overflow the buffer index
	if (CANBufferEntryPointer >= CAN_BUFFER_SIZE)
	{
		CANBufferEntryPointer = 0;
	}
	
	return 0;
}
	
////
/// Enqueues one CAN message to the circular buffer. Returns -1 if there is no space available in the buffer
char QueueCanMessageOutputCANBuffer(struct CanMessage c)
{
	if (CANBufferlength >= CAN_BUFFER_SIZE)//The buffer is full
	{
		return (-1);
	}
	
	//Queuing the CAN message
	OutputCANBuffer[CANBufferEntryPointer].Id = c.Id;
	OutputCANBuffer[CANBufferEntryPointer].DataLength = c.DataLength;
	OutputCANBuffer[CANBufferEntryPointer].Data[0] = c.Data[0];
	OutputCANBuffer[CANBufferEntryPointer].Data[1] = c.Data[1];
	OutputCANBuffer[CANBufferEntryPointer].Data[2] = c.Data[2];
	OutputCANBuffer[CANBufferEntryPointer].Data[3] = c.Data[3];
	OutputCANBuffer[CANBufferEntryPointer].Data[4] = c.Data[4];
	OutputCANBuffer[CANBufferEntryPointer].Data[5] = c.Data[5];
	OutputCANBuffer[CANBufferEntryPointer].Data[6] = c.Data[6];
	OutputCANBuffer[CANBufferEntryPointer].Data[7] = c.Data[7];
	
	//Increasing the entry pointer 
	CANBufferEntryPointer++;
	//Increasing the buffer length
	CANBufferlength++;

	//Check the pointer doesn't overflow the buffer index
	if (CANBufferEntryPointer >= CAN_BUFFER_SIZE)
	{
		CANBufferEntryPointer = 0;
	}
	
	return 0;
}

////
/// Sends one CAN message from the circular buffer if only if there is something to send.Returns -1 if there is no CAN message to send. Returns -2 if the previous CAN message is still being sent
char SendAndDequeueOneCanMessageOutputCANBuffer()
{
	unsigned long ExtId;	
	
	if (CANBufferlength == 0) //The buffer is empty
	{
		return -1;
	}

	if (C1TX0CONbits.TXREQ == 1) //In this case the previous CAN message is still being sent
	{
		return -2;
	}
	
	//while (C1TX0CONbits.TXREQ);
	
	//Sending Extended CAN message
	ExtId = (OutputCANBuffer[CANBufferExitPointer].Id) & (0x1FFC0000);
	ExtId = ExtId >> 18;
	
	CAN1SendMessage(CAN_TX_SID(ExtId) & CAN_TX_EID_EN & CAN_SUB_NOR_TX_REQ, 
					CAN_TX_EID(OutputCANBuffer[CANBufferExitPointer].Id) & CAN_NOR_TX_REQ,
					OutputCANBuffer[CANBufferExitPointer].Data, 
					OutputCANBuffer[CANBufferExitPointer].DataLength, 
					0);
				
	//Increasing the exit pointer 
	CANBufferExitPointer++;
	
	//Decreasing the buffer length
	CANBufferlength--;
	
	//Check the pointer doesn't overflow the buffer index
	if (CANBufferExitPointer >= CAN_BUFFER_SIZE)
	{
		CANBufferExitPointer = 0;
	}
	
	return 0;
}

////
/// Returns the number of CAN messages to be sent
char HowManyCANMessagesAreReadyToBeSent()
{
	return CANBufferlength;
	
	/*
	char n;
	n = CANBufferEntryPointer - CANBufferExitPointer;
	if (n == 0)
	{
		return n;
	}
	else
	{
		if (n < 0)
		{
			return n + CAN_BUFFER_SIZE;
		}
		else
		{
			return n;
		}
	}
	*/
	
	//Comprobar que lo anterior es lo mismo que esto:!!
	//return (CANBufferEntryPointer - CANBufferExitPointer)<0? ((CANBufferEntryPointer - CANBufferExitPointer)+CAN_BUFFER_SIZE) : (CANBufferEntryPointer - CANBufferExitPointer);
}

////
/// Fills the output CAN buffer with 'number' CAN messages 
/*
void TestOutputCANBuffer(int number)
{
	char i;	
	struct CanMessage CM;
	i=0;
	CM.Id = 0x123400;
	CM.DataLength = i+1;
	CM.Data[0] = i;
	CM.Data[1] = 0xFF-i;
	CM.Data[2] = 0x00;
	CM.Data[3] = 0xFF;
	CM.Data[4] = 0x00;
	CM.Data[5] = 0xFF-i;
	CM.Data[6] = i;
	CM.Data[7] = 0x10;
	
	for (i = 0; i<number; i++)
	{
		CM.Id = 0x123401 + i;
		CM.DataLength = i % 9;
		//putcUART2(QueueCanMessageOutputCANBuffer(CM));
		QueueCanMessageOutputCANBuffer(CM);
	}
	
	//QueueCanMessageOutputCANBuffer(CM);
	
	//CM.Id = 0x123400 + ++i;
	//QueueCanMessageOutputCANBuffer(CM);
	//CM.Id = 0x123400 + ++i;
	//QueueCanMessageOutputCANBuffer(CM);
	//CM.Id = 0x123400 + ++i;
	//QueueCanMessageOutputCANBuffer(CM);

}	
*/

////////////////////////////////
/// End Output CAN Buffer /////
//////////////////////////////


/////////////////////
/// CAN Device /////
///////////////////

#define NO_CAN_FILTER 0x00000000
#define FILT0_B0 0x00000000
#define FILT1_B0 0x00000000
#define FILT2_B1 0x00000000
#define FILT3_B1 0x00000000
#define FILT4_B1 0x00000000
#define FILT5_B1 0x00000000

////
/// CAN Initialization and configuration ///
void CAN1InitAndConfiguration(void)
{

	//Para reloj de 8 MHz externo, configuracion XT w/PLL 8x
	CAN1SetOperationMode(	CAN_IDLE_CON &
							CAN_MASTERCLOCK_0 &
							CAN_REQ_OPERMODE_CONFIG &
							CAN_CAPTURE_DIS);
							
	// Espera a que se actualicen los bits de configuraci�n del CAN
	while(C1CTRLbits.OPMODE <=3);

	// Configuracion para CAN a 50Kb/s (para reloj a 8 MHz externo, configuracion XT w/PLL 8x)
	CAN1Initialize( CAN_SYNC_JUMP_WIDTH1 &
                    CAN_BAUD_PRE_SCALE(16),
    	 			CAN_WAKEUP_BY_FILTER_DIS &
                    CAN_PROPAGATIONTIME_SEG_TQ(1) &
                    CAN_PHASE_SEG1_TQ(5) &
                    CAN_PHASE_SEG2_TQ(3) &
                    CAN_SEG2_FREE_PROG &
                    CAN_SAMPLE3TIMES);

	//CAN1 BUFFER 0
	CAN1SetMask(0,CAN_MASK_EID(NO_CAN_FILTER) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(FILT1_B0));
	CAN1SetFilter(0, CAN_FILTER_SID(FILT0_B0) & CAN_RX_EID_EN, CAN_FILTER_EID(FILT1_B0));
	CAN1SetFilter(1, CAN_FILTER_SID(FILT1_B0) & CAN_RX_EID_EN, CAN_FILTER_EID(FILT1_B0));
	
	//CAN1 BUFFER 1
	CAN1SetMask(1,CAN_MASK_EID(NO_CAN_FILTER) & CAN_MATCH_FILTER_TYPE,0);
	CAN1SetFilter(2,CAN_FILTER_EID(FILT2_B1) & CAN_RX_EID_EN,0);
	CAN1SetFilter(3,CAN_FILTER_EID(FILT3_B1) & CAN_RX_EID_EN,0);
	CAN1SetFilter(4,CAN_FILTER_EID(FILT4_B1) & CAN_RX_EID_EN,0);
	CAN1SetFilter(5,CAN_FILTER_EID(FILT5_B1) & CAN_RX_EID_EN,0);	
	
	CAN1SetRXMode(0,CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);
	CAN1SetRXMode(1,CAN_RXFUL_CLEAR);
	
	CAN1SetTXMode(0,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
	CAN1SetTXMode(1,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
	CAN1SetTXMode(2,CAN_TX_STOP_REQ & CAN_TX_PRIORITY_HIGH);
	
	CAN1SetOperationMode(CAN_REQ_OPERMODE_NOR);

	ConfigIntCAN1(	CAN_INDI_INVMESS_DIS &
					CAN_INDI_WAK_DIS &
					CAN_INDI_ERR_DIS &
					CAN_INDI_TXB0_EN &
					CAN_INDI_TXB1_DIS &
					CAN_INDI_TXB2_DIS &
					CAN_INDI_RXB0_EN &
					CAN_INDI_RXB1_DIS,
					CAN_INT_ENABLE &
					CAN_INT_PRI_4); //prioridad 1 la mas baja segun 70046D.pdf (prioridad 0 deshabilita interrupciones)
}
////
/// CAN_1 global interrupt service routine ///
void __attribute__((interrupt, auto_psv)) _C1Interrupt(void)
{
	unsigned long Id;
	int i;
	unsigned char datos[8];
	unsigned char DataLength;
	
	//transmision por buffer 0 CAN 1
	if (C1INTFbits.TX0IF == 1)
	{
		//se activa cuando se ha terminado de enviar por CAN 1
		C1INTFbits.TX0IF = 0;
		IFS1bits.C1IF = 0;
	}
	
	//recepcion por buffer 0 CAN 1
	if (C1INTFbits.RX0IF == 1)
	{
		//reseteamos los flags
		C1INTFbits.RX0IF = 0;
		IFS1bits.C1IF = 0;	

		//tratamiento de un mensaje CAN recibido
		//Manejamos el identificador dependiendo si es stantard o extended
		if (C1RX0SIDbits.RXIDE == 0) //es standard --> se enviar� al wiport como si fuera un extended
		{
			Id = C1RX0SIDbits.SID;
		}
		else //es extended
		{
			Id = (C1RX0DLCbits.EID5_0 + ((C1RX0EID & 0x0003)<<6));
			Id += ((unsigned long)((C1RX0EID & 0x03FC) >> 2))<<8;
			Id += ((unsigned long)(((C1RX0EID & 0x0C00) >> 10) + ((C1RX0SIDbits.SID & 0x03F) << 2)))<<16;
			Id += ((unsigned long)((C1RX0SIDbits.SID & 0x7C0) >> 6))<<24;
		}		
			 
		DataLength = C1RX0DLCbits.DLC;
		for (i=0; i<DataLength; i++)
		{
			datos[i] = *((unsigned char *)&C1RX0B1 + (i));
		}	
		
		////////////	    	    
		/// Echo sent as extended CAN frame ///
		//idExt = (Id) & (0x1FFC0000);
		//idExt = idExt >> 18;
		//CAN1SendMessage(CAN_TX_SID(idExt) & CAN_TX_EID_EN & CAN_SUB_NOR_TX_REQ, CAN_TX_EID(Id)& CAN_NOR_TX_REQ , datos, DataLength, 0);
		////////////
			
		////////////
		/// Echo sent as standard CAN frame ///
		//CAN1SendMessage(CAN_TX_SID(Id) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ, CAN_TX_EID(0)& CAN_NOR_TX_REQ, datos, DataLength, 0);
		////////////

		//Se resetea el flag de mensaje valido cuando ya se tiene
		C1RX0CONbits.RXFUL = 0;
		
		
		if ((Id == (ACTUATION_PERIOD_ID | CAN_NODE_ID)) ||
			(Id == (ACTUATION_PERIOD_ID | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		{
			//If the returned value is different than 0, then is required an ACK message
			if (ProcessActuatorMessages(datos, &DataLength))
			{	
				//Enqueue the CAN message
				datos[DataLength-1] = 0; //Reset the ACK bit
				QueueFieldsCanMessageOutputCANBuffer(ACK_ID | ACTUATION_PERIOD_ID | CAN_NODE_ID, DataLength, datos);
			}
			return;
		}
		
		if ((Id == (SENSORING_PERIOD_ID | CAN_NODE_ID)) ||
			(Id == (SENSORING_PERIOD_ID | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		{
			//If the returned value is different than 0, then is required an ACK message
			if (ProcessSensoringMessages(datos, &DataLength))
			{	
				//Enqueue the CAN message
				datos[DataLength-1] = 0; //Reset the ACK bit
				QueueFieldsCanMessageOutputCANBuffer(ACK_ID | SENSORING_PERIOD_ID | CAN_NODE_ID, DataLength, datos);
			}
			return;
		}

		if ((Id == (OPERATION_MODE_CAN_MSG_ID | CAN_NODE_ID)) ||
			(Id == (OPERATION_MODE_CAN_MSG_ID | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		{
			//If the returned value is different than 0, then is required an ACK message
			if (ProcessModeCANMessages(datos, &DataLength))
			{
				//Enqueue the CAN message
				datos[DataLength-1] = 0; //Reset the ACK bit
				QueueFieldsCanMessageOutputCANBuffer(ACK_ID | OPERATION_MODE_CAN_MSG_ID | CAN_NODE_ID, DataLength, datos);
			}
			return;
		}
		
		if ((Id == (SET_REFERENCE_ID | CAN_NODE_ID)) ||
			(Id == (SET_REFERENCE_ID | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		{
			//If the returned value is different than 0, then is required an ACK message
			if (ProcessReferenceCANMessages(datos, &DataLength))
			{
				//Enqueue the CAN message
				datos[DataLength-1] = 0; //Reset the ACK bit
				QueueFieldsCanMessageOutputCANBuffer(ACK_ID | SET_REFERENCE_ID | CAN_NODE_ID, DataLength, datos);
			}
			return;
		}
		
		//if ((Id == (ACTUACION_IN_CONTROL_CAN_MSG_ID | CAN_NODE_ID)) ||
		//	(Id == (ACTUACION_IN_CONTROL_CAN_MSG_ID | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		if ((Id&0x0F000000) == ACTUACION_IN_CONTROL_CAN_MSG_ID)
		{
			ProcessActuacionInControlCANMessages(Id, datos, &DataLength);
			return;	
		}
		
		if ((Id&0x0F000000) == SENSORING_IN_CONTROL_CAN_MSG_ID)
		{
			ProcessSensoringInControlCANMessages(Id, datos, &DataLength);
			return;
		}
		
		if ((Id == (PERIODIC_CONTROL_DATA_SENDING | CAN_NODE_ID)) ||
			(Id == (PERIODIC_CONTROL_DATA_SENDING | ((unsigned long)BROADCAST_NODE_ID<<8))) )
		{
			//If the returned value is different than 0, then is required an ACK message
			if (ProcessDataManagerMessages(datos, &DataLength))
			{
				//Enqueue the CAN message
				datos[DataLength-1] = 0; //Reset the ACK bit
				QueueFieldsCanMessageOutputCANBuffer(ACK_ID | PERIODIC_CONTROL_DATA_SENDING | CAN_NODE_ID, DataLength, datos);
			}
			return;
		}
		
		if (Id == CODE_LOADER_INPUT_ID)
		{			
			// Pasa el mensaje a la funci�n Cargador
			// Si devuelve un valor distinto de 0 es que hay que responder con otro mensaje
			if (CodeLoader(datos, &DataLength)) 
			{	
				//Encolamos la trama a enviar				
				QueueFieldsCanMessageOutputCANBuffer(CODE_LOADER_OUTPUT_ID, DataLength, datos);
			}
			return;
		}
	}
}
////
/// Sends a CAN message when the node is turn on in order to indicate to the network it is sitched on
void SendCANNodeSwitchOn()
{
	unsigned long Id;
	unsigned char Data[1];
	unsigned long ExtId;
		
	Id = NODE_SWITCH_ONOFF_CAN_ID | CAN_NODE_ID;
	Data[0] = 0x01;
	
	//Sending Extended CAN message
	ExtId = Id & 0x1FFC0000;
	ExtId = ExtId >> 18;
	
	CAN1SendMessage(CAN_TX_SID(ExtId) & CAN_TX_EID_EN & CAN_SUB_NOR_TX_REQ, 
					CAN_TX_EID(Id)& CAN_NOR_TX_REQ, 
					Data, 
					1, 
					0);
}
////
/// Sends a CAN message when the node is turn off in order to indicate to the network it is switched off
void SendCANNodeSwitchOff()
{
	unsigned long Id;
	unsigned char Data[1];
	unsigned long ExtId;
		
	Id = NODE_SWITCH_ONOFF_CAN_ID | CAN_NODE_ID;
	Data[0] = 0x00;
	
	//Sending Extended CAN message
	ExtId = Id & 0x1FFC0000;
	ExtId = ExtId >> 18;
	
	CAN1SendMessage(CAN_TX_SID(ExtId) & CAN_TX_EID_EN & CAN_SUB_NOR_TX_REQ, 
					CAN_TX_EID(Id)& CAN_NOR_TX_REQ, 
					Data, 
					1, 
					0);
}
/////////////////////////
/// End CAN Device /////
///////////////////////


//////////////////////
/// UART Device /////
////////////////////

#define UART_BAUD_RATE 		UART_BAUD_RATE_115200

////
/// UART2 Initialization and configuration ///
void UART2InitAndConfiguration(void)
{   
	TRISF |= 0x0010; //RF4-->in (1)
	TRISF &= 0xFFDF; //RF5-->out (0)
	U2MODE = 0xA000; //1 stop 8 bits par impar
	U2BRG = UART_BAUD_RATE;	
	U2STAbits.UTXEN = 1;
	U2STAbits.ADDEN=0;
	U2STAbits.UTXISEL=0;

	//UART TX interrupt priority
	IPC6bits.U2TXIP = 1;
	//UART RX interrupt priority
	IPC6bits.U2RXIP = 2;
}
////
/// UART_2 RX interrupt service routine ///
void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
{
	//this function will be executed when the UART2 receives a character 
	unsigned char c;
	
//	unsigned long id, idExt;
//	int i;
//	unsigned char datos[8];
//	unsigned char long_datos;
	
	c = getcUART2();
	
/*	
#ifdef DIGITAL_REFERENCE
	//Actualizar el valor de Reference
	
	//Valor recibido entre 0 y 255 (0x0 y 0xFF)
	//Reference between 0 and 1023
	SetProcessReference((unsigned int)(c*4));
	
//CON3bits.ADCS = c;

	//Reference between 0 and 255
	//Reference = (unsigned int)c;
	//Reference = c;
	
#endif
*/

	//do echo
	//putcUART2(c);
	
	/*
	datos[0] = 0xFF;
	datos[1] = 0xF8;
	long_datos = 2;
	if (c==ID_MENSAJE_ENTRADA)
		{
			//PORTC |= 0x4000; //LED B ON
				
			// Pasa el mensaje a la funci�n Cargador
			// Si devuelve un 1 es que hay que responder con otro mensaje
			if (cargador(datos, long_datos)==1) 
			{	
				id=ID_MENSAJE_SALIDA;
				CAN1SendMessage(CAN_TX_SID(id) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ, CAN_TX_EID(0)& CAN_NOR_TX_REQ, datos, long_datos, 0);
				//CAN1SendMessageNormal_1 (id11,identificador,datos_recib,long_datos);
			}
			
			//PORTC &= 0xBFFF; //LED B OFF
		}
	
	*/
	
	/*
	switch (c)
	{
		case '1':
			MainControlPtr = (void *) 0x002000;
			break;
		case '2':
			MainControlPtr = (void *) 0x003000;
			break;
		default:
			MainControlPtr = (void *) 0x000000;
			break;
	}
	*/
	
//PARA PROVOCAR UN ERROR EN CONTROL LOCAL
if (c == 0xFF)
{
	SetFail = 1;
}
	

	IFS1bits.U2RXIF = 0; //clear the interruption flag in order to be able to set the flag again
}
////
/// UART_2 TX interrupt service routine ///
void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
{  	
	//this function will be executed when the UART2 finishes to send a character
	IFS1bits.U2TXIF = 0; //clear the interruption flag in order to be able to set the flag again
}
////
/// Sends a 'hello' message through UART_2 ///
void SendHelloUART2(void)
{
	//UART2 sending example (there is no sofware buffer at the moment)	
	putcUART2('h');
	putcUART2('i');
	putcUART2('!');
	putcUART2('!');
}
////
/// Sends a 'bye' message through UART_2 ///
void SendByeUART2(void)
{
	//UART2 sending example (there is no sofware buffer at the moment)
	putcUART2('b');
	putcUART2('y');
	putcUART2('e');
}
////
/// Sends a 'init' message through UART_2 ///
void SendInitUART2(void)
{
	//UART2 sending example (there is no sofware buffer at the moment)
	putcUART2('i');
	putcUART2('n');
	putcUART2('i');
	putcUART2('t');
}
//////////////////////////
/// End UART Device /////
////////////////////////

////
/// Initializes the Data Manager Service of the Tiny Middleware ///
void  NetworkServiceInit(void)
{	
	//Hardware init
	UART2InitAndConfiguration();
	CAN1InitAndConfiguration();
	
	EnableIntCAN1;
	DisableIntU2TX;
	EnableIntU2RX;

	//Data init
	OutputCANBufferInit();
}






