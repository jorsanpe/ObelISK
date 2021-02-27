package Interface;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.LinkedList;

public class LogParser {
	private File logFile;
	private LinkedList<LogEntry> entries;
	private LinkedList<OThread> threads;

	private static int DWordToInt(byte[] bytes) {
		int value;

		value = ((bytes[3] << 24) & 0xff000000)
				| ((bytes[2] << 16) & 0x00ff0000)
				| ((bytes[1] << 8) & 0x0000ff00) | (bytes[0] & 0x000000ff);

		return value;
	}

	private static short WordToInt(byte[] bytes) {
		int value;

		value = ((bytes[1] << 8) & 0x0000ff00) | (bytes[0] & 0x000000ff);

		return (short) value;
	}

	private byte[] readFromIdx(byte[] bytes, int idx, int size) {
		byte[] buf = new byte[size];
		for (int i = 0; i < size; ++i) {
			buf[i] = bytes[idx];
			idx++;
		}
		return buf;
	}
	
	public int getInitialTime() {
		return entries.get(0).time;
	}
	
	public int getFinalTime() {
		return entries.get(entries.size()-1).time;
	}
	
	public LinkedList<OThread> getThreadList() {
		return threads;
	}

	public int parseLogFile(File log) {
		FileInputStream in = null;
		byte[] logStream = new byte[(int) log.length()];
		byte[] tmpbuf = new byte[4];
		LinkedList<LogEntry> tmpentries = new LinkedList<LogEntry>();
		entries = new LinkedList<LogEntry>();
		LogEntry newentry = null;
		int c, k;

		try {
			in = new FileInputStream(log.getAbsolutePath());

			// Read log file contents
			k = 0;
			while ((c = in.read()) != -1) {
				logStream[k] = (byte) c;
				k++;
			}

			// Read entries
			c = 0;
			while (c < k) {
				newentry = new LogEntry();

				// Read time
				tmpbuf = readFromIdx(logStream, c, 4);
				newentry.time = DWordToInt(tmpbuf) * 1000;
				c += 4;

				// Read event
				tmpbuf = readFromIdx(logStream, c, 2);
				newentry.event = WordToInt(tmpbuf);
				c += 2;

				// Read arg1
				tmpbuf = readFromIdx(logStream, c, 2);
				newentry.arg1 = WordToInt(tmpbuf);
				c += 2;

				// Read arg2
				tmpbuf = readFromIdx(logStream, c, 2);
				newentry.arg2 = WordToInt(tmpbuf);
				c += 2;

				// Read arg3
				tmpbuf = readFromIdx(logStream, c, 2);
				newentry.arg3 = WordToInt(tmpbuf);
				c += 2;

				// Insert the new entry in the list
				if (newentry.event != 0) {
					tmpentries.add(newentry);
				}
			}

			in.close();
			
			// Sort entry list
			int stidx = 0;
			for (int i=0; i<tmpentries.size()-1; i++) {
				LogEntry prev = tmpentries.get(i);
				LogEntry next = tmpentries.get(i+1);
				if (next.time < prev.time) {
					stidx = i+1;
					break;
				}
			}
			
			for (int i=0; i<tmpentries.size(); ++i) {
				LogEntry ent = tmpentries.get(stidx);
				//ent.printEntry();
				entries.add(ent);
				stidx = (stidx + 1) % tmpentries.size();
			}

		} catch (FileNotFoundException e) {
			return -1;
		} catch (IOException e) {
			return -2;
		}

		return 0;
	}
	
	public LinkedList<LogEntry> getEntries() {
		return this.entries;
	}

	public OThread getThread(int id) {
		for (int i = 0; i < threads.size(); i++) {
			OThread thread = threads.get(i);
			if (thread.getID() == id) {
				return thread;
			}
		}
		return null;
	}

	public boolean isThread(int id) {
		for (int i = 0; i < threads.size(); i++) {
			OThread thread = threads.get(i);
			if (thread.getID() == id) {
				return true;
			}
		}
		return false;
	}

	public void analyzeThreads() {

		threads = new LinkedList<OThread>();
		OThread thread;

		// Insert the main and the idle thread into the thread list
		thread = new OThread(OThread.MAIN_THREAD);
		threads.add(thread);
		thread = new OThread(OThread.IDLE_THREAD);
		threads.add(thread);

		// Create all the threads
		for (int i = 0; i < entries.size(); i++) {
			LogEntry entry = entries.get(i);
			
			switch (entry.event) {
			case Event.CREATE_TASK:
			case Event.CREATE_TASK_AID:
				thread = new OThread(entry.arg1);
				threads.add(thread);
				break;
				
			case Event.SCHEDULER:
			case Event.INSERT_READYQ:
			case Event.REMOVE_READYQ:
				if (getThread(entry.arg1) == null) {
					thread = new OThread(entry.arg1);
					threads.add(thread);
				}
			}
		}
	}
	
	public void createThreadEventtable() {
		int active_thread_id = OThread.MAIN_THREAD;
		OThread thread;
		
		// Find the current active thread
		// By default, we assume that main thread is active
		for (int i = 0; i < entries.size(); i++) {
			LogEntry entry = entries.get(i);
			if (entry.event == Event.SCHEDULER) {
				active_thread_id = entry.arg1;
				break;
			}
		}
		
		// Create table of events and states
		thread = getThread(active_thread_id);
		thread.current_state = State.TASK_RUN;
		State s = new State(State.TASK_RUN, 0);
		thread.addState(s);
		int currentTime = entries.get(0).time;
		for (int i = 0; i < entries.size(); i++) {
			LogEntry entry = entries.get(i);
			if (entry.time > currentTime) {
				currentTime = entry.time;
			}
			if (Event.isEvent(entry.event)) {
				if (Event.changesState(entry.event)) {
					
					// Scheduler Call
					if (entry.event == Event.SCHEDULER) {
						
						// Task preempted due to a higher priority task
						// New state -> ready
						thread = getThread(active_thread_id);
						if (thread.current_state == State.TASK_RUN) {
							thread.current_state = State.TASK_READY;
							thread.addState(new State(State.TASK_READY, currentTime));
						}
						
						// Set running task state 
						active_thread_id = entry.arg2;
						thread = getThread(active_thread_id);
						thread.current_state = State.TASK_RUN;
						thread.addState(new State(State.TASK_RUN, currentTime));
					}

					// Thread removed from ready queue
					if (entry.event == Event.REMOVE_READYQ) {
						thread = getThread(entry.arg1);
						thread.current_state = entry.arg2; 
						thread.addState(new State(entry.arg2, currentTime));
					}
					
					// Thread inserted into ready queue
					if (entry.event == Event.INSERT_READYQ) {
						thread = getThread(entry.arg1);
						thread.current_state = State.TASK_READY; 
						thread.addState(new State(State.TASK_READY, currentTime));
					}
					
					// Thread created
					if (entry.event == Event.CREATE_TASK
						|| entry.event == Event.CREATE_TASK_AID) {
						thread = getThread(entry.arg1);
						thread.current_state = State.TASK_DORMANT;
						thread.addState(new State(State.TASK_NIL, 0));
						thread.addState(new State(State.TASK_DORMANT, currentTime));
						thread.addEvent(new Event(entry));
					}
					
					// Thread deleted
					if (entry.event == Event.DELETE_TASK
						|| entry.event == Event.EXIT_TASK) {
						thread = getThread(entry.arg1);
						thread.current_state = State.TASK_NIL;
						thread.addState(new State(State.TASK_NIL, currentTime));
						thread.addEvent(new Event(entry));
					}
				} else {
					thread = getThread(active_thread_id);
					Event e = new Event(entry);
					e.time = currentTime;
					thread.addEvent(e);
				}
			}
			currentTime ++;
		}
	}
}
