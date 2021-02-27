package Interface;

import java.util.LinkedList;

public class OThread {
	public static final int MAIN_THREAD = 0;
	public static final int IDLE_THREAD = 5;
	
	private int ID;
	public int current_state;
	private LinkedList<State> states;
	private LinkedList<Event> events;
	private String threadName = null;
	
	public OThread(int id) {
		this.events = new LinkedList<Event>();
		this.states = new LinkedList<State>();
		this.ID = id;
		this.threadName = new String("Thread " + id);
	}
	
	public int getID() {
		return this.ID;
	}
	
	public String getName() {
		return this.threadName;
	}
	
	public void addEvent(Event e) {
		events.add(e);
	}
	
	public void addState(State s) {
		states.add(s);
	}
	
	public LinkedList<State> getStates() {
		return this.states;
	}
	
	public LinkedList<Event> getEvents() {
		return this.events;
	}
	
	public int getStateAt(int time) {
		int state = states.get(0).state;
		for (int i=1; i<states.size(); i++) {
			if (states.get(i).time > time) {
				break;
			}
			state = states.get(i).state;
		}
		return state;
	}
	
	public LinkedList<Event> getEventsAt(int time) {
		LinkedList<Event> elist = new LinkedList<Event>();
		
		for (int i=0; i<events.size(); i++) {
			Event e = events.get(i);
			if (e.time == time) {
				elist.add(e);
			}
		}
		return elist;
	}
}



