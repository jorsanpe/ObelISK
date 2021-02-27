package Interface;

public class LogEntry {
	int time;
	short event;
	short arg1;
	short arg2;
	short arg3;
	
	public LogEntry() {
		
	}
	
	public void printEntry() {
		System.out.println("Time: " + this.time);
		System.out.println("Event: " + this.event);
		System.out.println("Arg 1: " + this.arg1);
		System.out.println("Arg 2: " + this.arg2);
		System.out.println("Arg 3: " + this.arg3);
	}
}
