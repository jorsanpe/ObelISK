package Interface;

public class State {
	public static final int TASK_NIL = 0x00;
	public static final int TASK_RUN = 0x01;
	public static final int TASK_READY = 0x02;
	public static final int TASK_WAITING = 0x04;
	public static final int TASK_SUSPENDED = 0x08;
	public static final int TASK_WAIT_SUSPEND = 0x0C;
	public static final int TASK_DORMANT = 0x010;
	
	public int state;
	public int time;
	
	public State(int state, int time) {
		this.state = state;
		this.time = time;
	}
	
	/*public int getStateFromEvent(int event) {
		switch (event) {
		
		}
	}*/
}
