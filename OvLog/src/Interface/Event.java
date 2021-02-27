package Interface;

public class Event {
	public static final int SCHEDULER 		= -0x01;
	public static final int INSERT_READYQ 	= -0x02;
	public static final int REMOVE_READYQ 	= -0x03;
	public static final int CREATE_TASK 	= -0x05;
	public static final int DELETE_TASK 	= -0x06;
	public static final int ACTIVATE_TASK 	= -0x07;
	public static final int EXIT_TASK 		= -0x0A;
	public static final int SLEEP_TASK 		= -0x11;
	public static final int SLEEP_TASK_TMO	= -0x12;
	public static final int CREATE_SEMAPHORE 	 = -0x21;
	public static final int SIGNAL_SEMAPHORE	 = -0x23;
	public static final int WAIT_SEMAPHORE		 = -0x25;
	public static final int SEND_TO_MAILBOX	  	 = -0x3F;
	public static final int RECEIVE_FROM_MAILBOX = -0x41;
	public static final int CREATE_TASK_AID 	 = -0xC1;
	public static final int CREATE_SEMAPHORE_AID = -0xC2;
	public static final int CREATE_MAILBOX_AID   = -0xC5;
	
	public int time;
	public int event;
	public int arg1;
	public int arg2;
	public int arg3;
	
	public Event(LogEntry entry) {
		this.time = entry.time;
		this.event = entry.event;
		this.arg1 = entry.arg1;
		this.arg2 = entry.arg2;
		this.arg3 = entry.arg3;
	}
	
	public static boolean isEvent(int val) {
		switch (val) {
		case SCHEDULER:
		case INSERT_READYQ:
		case REMOVE_READYQ:
		case CREATE_TASK:
		case DELETE_TASK:
		case ACTIVATE_TASK:
		case EXIT_TASK:
		case SLEEP_TASK:
		case SLEEP_TASK_TMO:
		case CREATE_SEMAPHORE:
		case SIGNAL_SEMAPHORE:
		case WAIT_SEMAPHORE:
		case SEND_TO_MAILBOX:
		case RECEIVE_FROM_MAILBOX:
		case CREATE_TASK_AID:
		case CREATE_SEMAPHORE_AID:
		case CREATE_MAILBOX_AID:
			return true;
		}
		return false;
	}
	
	public static boolean changesState(int val) {
		switch (val) {
		case SCHEDULER:
		case INSERT_READYQ:
		case REMOVE_READYQ:
		case CREATE_TASK:
		case CREATE_TASK_AID:
		case DELETE_TASK:
		case EXIT_TASK:
			return true;
		}
		return false;
	}
	
}
