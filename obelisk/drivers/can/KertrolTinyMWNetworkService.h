
#ifndef KertrolTinyMWNetworkService_h
	#define KertrolTinyMWNetworkService_h

	#define CAN_NODE_ID						((unsigned long)MSB_NODE_ID<<16) | ((unsigned long)LSB_NODE_ID<<8)

	#define OPERATION_MODE_CAN_MSG_ID		0x08000000	
	#define ACTUACION_IN_CONTROL_CAN_MSG_ID	0x01000000
	#define SENSORING_IN_CONTROL_CAN_MSG_ID	0x02000000
	
	#define CONTROL_ALARM_CAN_MSG_ID		0x09000000
	#define SET_REFERENCE_ID				0x03000000

	#define PERIODIC_CONTROL_DATA_SENDING	0x04000000
		
	#define REFERENCE_INFO_CAN_MSG_ID		0x05000000
	#define ACTUACION_INFO_CAN_MSG_ID		0x06000000
	#define SENSORING_INFO_CAN_MSG_ID		0x07000000
	
	#define ACTUATION_PERIOD_ID				0x0C000000
	#define SENSORING_PERIOD_ID				0x0D000000
		
	#define CODE_LOADER_INPUT_ID			0x0F000030
	#define CODE_LOADER_OUTPUT_ID			0x0F000031
	
	#define NODE_SWITCH_ONOFF_CAN_ID		0x0A000000
	
	#define ACK_ID							0x10000000
	
	#define CAN_BUFFER_SIZE					64

	//BRG for extern 8 MHz cristal, XT w/PLL 8x configuration (Fcy = ((8*8)/4) = 16 MHz)
	#define UART_BAUD_RATE_9600 103 //BRG = (16e6/(16*BaudRate))-1  if we want BR = 9600 bps --> BRG = 103,166
	#define UART_BAUD_RATE_14400 68 //BRG = (16e6/(16*BaudRate))-1  if we want BR = 14400 bps --> BRG = 68,444
	#define UART_BAUD_RATE_19200 51 //BRG = (16e6/(16*BaudRate))-1  if we want BR = 19200 bps --> BRG = 51,083
	#define UART_BAUD_RATE_38400 25 //BRG = (16e6/(16*BaudRate))-1  if we want BR = 38400 bps --> BRG = 25,041
	#define UART_BAUD_RATE_57600 16 //BRG = (14,74e6/(16*BaudRate))-1  if we want BR = 57600 bps --> BRG = 16,361
	#define UART_BAUD_RATE_115200 8 //BRG = (14,74e6/(16*BaudRate))-1  if we want BR = 115200 bps --> BRG = 7,680


	struct CanMessage
	{
		unsigned long Id;
		unsigned char DataLength;
		unsigned char Data[8];
	};
	
	////
	/// Enqueues the fields of one CAN message to the circular buffer. Returns -1 if there is no space available in the buffer ///
	char QueueFieldsCanMessageOutputCANBuffer(unsigned long Id, unsigned char DataLength, unsigned char Data[8]);
	////
	/// Enqueues one CAN message to the circular buffer. Returns -1 if there is no space available in the buffer ///
	char QueueCanMessageOutputCANBuffer(struct CanMessage c);
	////
	/// Sends one CAN message from the circular buffer if only if there is something to send.Returns -1 if there is no CAN message to send. Returns -2 if the previous CAN message is still being sent ///
	char SendAndDequeueOneCanMessageOutputCANBuffer();
	////
	/// Returns the number of CAN messages to send ///
	char HowManyCANMessagesAreReadyToBeSent();
	
	////
	/// Sends a 'hello' message through UART_2 ///
	void SendHelloUART2(void);
	////
	/// Sends a 'bye' message through UART_2 ///
	void SendByeUART2(void);
	////
	/// Sends a 'init' message through UART_2 ///
	void SendInitUART2(void);

	////
	/// Initializes the Data Manager Service of the Tiny Middleware ///
	void NetworkServiceInit(void);
	
	////
	/// Sends a CAN message when the node is turn on in order to indicate to the network it is switched on
	void SendCANNodeSwitchOn();
	////
	/// Sends a CAN message when the node is turn off in order to indicate to the network it is switched off
	void SendCANNodeSwitchOff();
	
	
#endif


