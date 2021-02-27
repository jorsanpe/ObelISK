package Interface;
/*****************************************************************************

 JEP 2.4.1, Extensions 1.1.1
      April 30 2007
      (c) Copyright 2007, Nathan Funk and Richard Morris
      See LICENSE-*.txt for license information.

*****************************************************************************/


import java.awt.BasicStroke;
import java.awt.Canvas;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.util.LinkedList;

/**
 * This class plots a graph using the JEP API.
 */
public class GraphCanvas extends Canvas {
	private static final long serialVersionUID = -3169263228971794887L;
	private static final int ZOOM_STATES = 1;
	private static final int ZOOM_EVENTS = 2;
	private static final int MARGIN_LEFT = 80;
	private static final int MARGIN_RIGHT = 15;

	/** Dimensions of the canvas */
	private Dimension dimensions;
	private int minX, maxX, drawSize;

	/** Buffer for the graph */
	private Image buffer;

	/** Boolean flags */
	private boolean initializedBuffer, changedFunction, hasError;
	
	/** Scale variables */
	private int scaleY;
	private float scaleX;
	private int initialTime, finalTime;
	
	/** Zooming states */
	private int zoomingX, zoomingY, zoomingDrag;
	private boolean zooming = false;
	private int zoomState = GraphCanvas.ZOOM_STATES;
	
	/** Thread and events List */
	LinkedList<OThread> threads = null;
	LinkedList<LogEntry> entries = null;
	
	/**
	 * Constructor
	 */
	public GraphCanvas() {
		dimensions = getSize();
		initializedBuffer = false;
		changedFunction = true;
		hasError = true;
		scaleY = dimensions.height / 10;
	}

	/**
	 * Fills the background with white.
	 */
	private void paintWhite(Graphics g) {
		g.setColor(Color.white);
		g.fillRect(0,0,dimensions.width,dimensions.height);
	}
	
	/*
	 * Color-State table
	 */
	private Color getStateColor(int state) {
		switch (state) {
		case State.TASK_NIL:
			return Color.white;
		case State.TASK_DORMANT:
			return Color.gray;
		case State.TASK_READY:
			return Color.blue;
		case State.TASK_RUN:
			return Color.green;
		case State.TASK_WAITING:
		case State.TASK_SUSPENDED:
		case State.TASK_WAIT_SUSPEND:
			return Color.ORANGE;
		}
		return Color.white;
	}
	
	public void setLogData(LinkedList<OThread> threads, LinkedList<LogEntry> entries) {
		this.threads = threads;
		scaleY = Math.min(dimensions.height / (threads.size()+1), 70);
	}
	
	public void setTimeScale(int ini, int fin) {
		
		// Set drawing features
		minX = MARGIN_LEFT;
		maxX = dimensions.width - MARGIN_RIGHT;
		drawSize = maxX - minX;
		
		if (fin - ini > 10) {
			// Margin to the initial and final times
			this.initialTime = ini;
			this.finalTime = fin;
			scaleX = (float)(finalTime - initialTime) / drawSize;
		}
		
		// Zooming very close enables state zoom
		/*if (ini == fin) {
			zoomState = GraphCanvas.ZOOM_EVENTS;
		} else {
			zoomState = GraphCanvas.ZOOM_STATES;
			scaleX = (float)(finalTime - initialTime) / dimensions.width;
		}*/
	}
	
	private int rel2abs(int relx) {
		return (int)((relx - initialTime) / scaleX) + MARGIN_LEFT + MARGIN_RIGHT;
	}
	
	private int abs2rel(int absx) {
		return (int)((absx - (MARGIN_LEFT+MARGIN_RIGHT)) * scaleX) + initialTime;
	}
	
	private void paintRect(Graphics2D g, int x, int y, int sx, int sy) {
		if (g.getColor() == Color.white) {
			return;
		}
		g.fillRect(x, y, sx, sy);
		g.setColor(Color.black);
		g.drawRect(x, y, sx, sy);
	}
	
	private void paintTimeline(Graphics2D g, int currentY) {
		int timescale = 1;
		int mintick, ticksize;
		float stretch;
		
		// Paint time line
		g.setStroke(new BasicStroke(1.5f));
		g.setColor(Color.darkGray);
		g.drawLine(minX, currentY, maxX, currentY);
		g.drawLine(minX, currentY, minX, currentY-(scaleY/4)+5);
		g.setFont(new Font("SansSerif", Font.PLAIN, 10));
		
		if (finalTime - initialTime > 1000) {
		
			// Adjust time line scale
			while (finalTime - initialTime > timescale) {
				timescale *= 10;
			}
			mintick = Math.max(timescale / 100, 1);
			stretch = (float)(finalTime - initialTime) / timescale;
			
			if (stretch < 0.25) {
				ticksize = mintick;
			} else if (stretch < 0.75) {
				ticksize = mintick * 5;
			} else {
				ticksize = mintick * 10; 
			}
			
			//g.drawString("0", 0, currentY+15);
			
			int pos = ticksize;
			while (pos < finalTime) {
				g.drawLine(rel2abs(pos), currentY, rel2abs(pos), currentY-(scaleY/4)+5);
				g.drawString("" + pos/1000, rel2abs(pos)-10, currentY+15);
				pos += ticksize;
			}
		} else {
			int pos = (finalTime + initialTime) / 2;
			g.drawLine(rel2abs(pos), currentY, rel2abs(pos), currentY-(scaleY/4)+5);
			g.drawString("" + pos/1000, rel2abs(pos)-10, currentY+15);
		}
	}
	
	public void startZoomAction(int x, int y) {
		this.zoomingX = x;
		this.zoomingY = y;
		this.zoomingDrag = x;
		zooming = true;
	}
	
	public void updateZoomAction(int x) {
		this.zoomingDrag = x;
	}
	
	public void endZoomAction(int x) {
		setTimeScale(abs2rel(zoomingX), abs2rel(x));
		zooming = false;
	}
	
	private void paintZoomLine(Graphics2D g) {
		g.setStroke(new BasicStroke(1.5f));
		g.setColor(Color.gray);
		g.drawLine(zoomingX, zoomingY-10, zoomingX, zoomingY+10);
		g.drawLine(zoomingDrag, zoomingY-10, zoomingDrag, zoomingY+10);
		g.drawLine(zoomingX, zoomingY, zoomingDrag, zoomingY);
	}
	
	private void drawArrow( Graphics2D g, int x, int y, int xx, int  yy, Color c)
	  {
	    float arrowWidth = 10.0f ;
	    float theta = 0.423f ;
	    int[] xPoints = new int[ 3 ] ;
	    int[] yPoints = new int[ 3 ] ;
	    float[] vecLine = new float[ 2 ] ;
	    float[] vecLeft = new float[ 2 ] ;
	    float fLength;
	    float th;
	    float ta;
	    float baseX, baseY ;

	    xPoints[ 0 ] = xx ;
	    yPoints[ 0 ] = yy ;

	    // build the line vector
	    vecLine[ 0 ] = (float)xPoints[ 0 ] - x ;
	    vecLine[ 1 ] = (float)yPoints[ 0 ] - y ;

	    // build the arrow base vector - normal to the line
	    vecLeft[ 0 ] = -vecLine[ 1 ] ;
	    vecLeft[ 1 ] = vecLine[ 0 ] ;

	    // setup length parameters
	    fLength = (float)Math.sqrt( vecLine[0] * vecLine[0] + vecLine[1] * vecLine[1] ) ;
	    th = arrowWidth / ( 2.0f * fLength ) ;
	    ta = arrowWidth / ( 2.0f * ( (float)Math.tan( theta ) / 2.0f ) * fLength ) ;

	    // find the base of the arrow
	    baseX = ( (float)xPoints[ 0 ] - ta * vecLine[0]);
	    baseY = ( (float)yPoints[ 0 ] - ta * vecLine[1]);

	    // build the points on the sides of the arrow
	    xPoints[ 1 ] = (int)( baseX + th * vecLeft[0] );
	    yPoints[ 1 ] = (int)( baseY + th * vecLeft[1] );
	    xPoints[ 2 ] = (int)( baseX - th * vecLeft[0] );
	    yPoints[ 2 ] = (int)( baseY - th * vecLeft[1] );

	    g.setColor(c);
	    g.drawLine( x, y, (int)baseX, (int)baseY ) ;
	    g.fillPolygon( xPoints, yPoints, 3 ) ;
	    g.setColor(Color.black);
	    g.drawPolygon( xPoints, yPoints, 3 ) ;
	  }
	
	private void paintStates(Graphics2D g) {
		int currentY = 20;
		
		g.setFont(new Font("SansSerif", Font.BOLD, 12));
		for (int i=0; i<threads.size(); i++) {
			OThread thread = threads.get(i);
			LinkedList<State> states = thread.getStates();
			LinkedList<Event> events = thread.getEvents();
			
			g.drawString(thread.getName(), 5, currentY + (scaleY / 2));
			
			// Paint Thread States
			int bminx, bminy, bsizx, bsizy;
			bminx = Math.max(minX, rel2abs(0));
			bminy = currentY + (scaleY / 4);
			bsizy = scaleY / 2;
			State s = states.get(0);
			g.setColor(getStateColor(s.state));
			for (int j=1; j<states.size(); j++) {
				s = states.get(j);
				bsizx = rel2abs(s.time) - bminx;
				paintRect(g, bminx, bminy, bsizx, bsizy);
				g.setColor(getStateColor(s.state));
				bminx = bminx + bsizx;
			}
			bsizx = maxX - bminx;
			paintRect(g, bminx, bminy, bsizx, bsizy);
			currentY += scaleY;
			
			// Paint thread events
			int ctime, ptime;
			ctime = ptime = 0;
			for (int j=0; j<events.size(); j++) {
				Event e = events.get(j);
				drawArrow(g, rel2abs(e.time), bminy-10, rel2abs(e.time), bminy, Color.gray);
				ctime ++;
			}
		}
	}
	
	private void paintEvents(Graphics2D g) {
		int currentY = 20;
		
		for (int i=0; i<threads.size(); i++) {
			OThread thread = threads.get(i);
			LinkedList<Event> events = thread.getEventsAt(initialTime);
			
			// Paint thread state
			int bminx, bminy, bsizx, bsizy;
			bminx = MARGIN_LEFT;
			bminy = currentY + (scaleY / 4);
			bsizx = dimensions.width;
			bsizy = scaleY / 2;
			g.setColor(getStateColor(thread.getStateAt(initialTime)));
			paintRect(g, bminx, bminy, bsizx, bsizy);
			currentY += scaleY;
			
			// Paint thread events
			int currentX = 10;
			for (int j=0; j<events.size(); j++) {
				drawArrow(g, currentX, bminy-10, currentX, bminy, Color.gray);
				currentX += 20;
			}
		}
	}

	/**
	 * Draws the graph to the Graphics object. If the image buffer has been
	 * initialized, and the function has not changed since the last paint, 
	 * the image stored in the buffer is drawn straight to the Graphics
	 * object with drawImage().
	 * <p>
	 * If a image buffer has not yet been initialized (i.e. first time after
	 * being started) the buffer is created with createImage().
	 * <p>
	 * If the function has changed since the last paint, the graph is first 
	 * drawn on the buffer image, then that image is drawn on the Graphics
	 * object.
	 */
	public void paint(Graphics g_in) {
		boolean changedDimensions = !dimensions.equals(getSize());
		Graphics2D g = (Graphics2D) g_in;
		
		// If the buffer has not been initialized, do it now
		if (!initializedBuffer || changedDimensions)
		{
			dimensions = getSize();
			buffer = createImage(dimensions.width,dimensions.height);
			initializedBuffer = true;
		}
		
		// Get the Graphics instance of the buffer
		Graphics2D buffergc = (Graphics2D) buffer.getGraphics();
		// Turn on anti aliasing
		buffergc.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		paintWhite(buffergc);
		
		if (threads != null) {
			//if (zoomState == GraphCanvas.ZOOM_STATES) {
				paintStates(buffergc);
			/*} else if (zoomState == GraphCanvas.ZOOM_EVENTS) {
				paintEvents(buffergc);
			}*/
			paintTimeline(buffergc, dimensions.height - scaleY);
			if (zooming) {
				paintZoomLine(buffergc);
			}
		}

		// Copy the buffer to g
		g.drawImage(buffer, 0, 0, null);
	}
}
