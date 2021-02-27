package Interface;
import Interface.GraphCanvas;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.text.DecimalFormat;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EtchedBorder;
import javax.swing.JCheckBox;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

public class AppInterface extends JFrame {
	private static final long serialVersionUID = 1L;
	private static final int MOUSE_ZOOM = 1;
	
	private JPanel jContentPane = null;
	private JPanel jPanelDisplay = null;
	private ImageIcon icon = null;
	private GraphCanvas graphCanvas;
	private JMenuBar mbOvLog = null;
	private JMenu mAbout = null;
	private JMenu mFile = null;
	private JMenuItem miOpenLogFile = null;
	private LogParser parser = null;
	private JMenuItem miExit = null;
	private int screenAction = 0;
	private int zoomIni, zoomEnd;
	private JPopupMenu pmPopUpMenu = null;
	private JMenuItem miPUResetZoom = null;
	private JMenu mTools = null;
	private JMenuItem miResetZoom = null;
	private void rebuildGraph() {
		DecimalFormat format = new DecimalFormat("0.0000");
		
		graphCanvas.repaint();
	}

	/**
	 * This method initializes jPanelDisplay	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanelDisplay() {
		if (jPanelDisplay == null) {
			graphCanvas = new GraphCanvas();
			graphCanvas.setBounds(new Rectangle(0, 0, 900, 600));
			graphCanvas.addMouseListener(new java.awt.event.MouseAdapter() {     
				public void mousePressed(java.awt.event.MouseEvent e) {
					if (e.getButton() == MouseEvent.BUTTON1) {
						screenAction = AppInterface.MOUSE_ZOOM;
						graphCanvas.startZoomAction(e.getX(), e.getY());
						graphCanvas.repaint();
					}
					if (e.getButton() == MouseEvent.BUTTON2) {
						pmPopUpMenu.show(graphCanvas, e.getX(), e.getY());
					}
				}
				public void mouseReleased(java.awt.event.MouseEvent e) {
					graphCanvas.endZoomAction(e.getX());
					graphCanvas.repaint();
				} 
			});
			graphCanvas.addMouseMotionListener(new java.awt.event.MouseMotionAdapter() {
				public void mouseDragged(java.awt.event.MouseEvent e) {
					graphCanvas.updateZoomAction(e.getX());
					graphCanvas.repaint();
				}
			});
			jPanelDisplay = new JPanel();
			jPanelDisplay.setBounds(new Rectangle(0, 0, 900, 600));
			jPanelDisplay.setLayout(null);
			jPanelDisplay.setPreferredSize(new Dimension(900, 600));
			jPanelDisplay.add(graphCanvas, null);
		}
		return jPanelDisplay;
	}

	/**
	 * This method initializes icon	
	 * 	
	 * @return javax.swing.ImageIcon	
	 */
	/*private ImageIcon getIcon() {
		if (icon == null) {
			icon = createImageIcon("f_d_x copia.png", "f(x)");
		}
		return icon;
	}*/

	/**
	 * This method initializes mbOvLog	
	 * 	
	 * @return javax.swing.JMenuBar	
	 */
	private JMenuBar getMbOvLog() {
		if (mbOvLog == null) {
			mbOvLog = new JMenuBar();
			mbOvLog.setPreferredSize(new Dimension(900, 21));
			mbOvLog.add(getMFile());
			mbOvLog.add(getMTools());
			mbOvLog.add(getMAbout());
		}
		return mbOvLog;
	}

	/**
	 * This method initializes mAbout	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getMAbout() {
		if (mAbout == null) {
			mAbout = new JMenu();
			mAbout.setText("About");
		}
		return mAbout;
	}

	/**
	 * This method initializes mFile	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getMFile() {
		if (mFile == null) {
			mFile = new JMenu();
			mFile.setText("File");
			mFile.add(getMiOpenLogFile());
			mFile.add(getMiExit());
		}
		return mFile;
	}

	/**
	 * This method initializes miOpenLogFile	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiOpenLogFile() {
		if (miOpenLogFile == null) {
			miOpenLogFile = new JMenuItem();
			miOpenLogFile.setText("Open Log File...");
			miOpenLogFile.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					// TODO Abrir diálogo selección fichero y parsearlo
					//Create a file chooser
					final JFileChooser fc = new JFileChooser("E:\\Trabajo\\Proyectos\\eclipse\\OvLog");
					fc.setControlButtonsAreShown(true);
					int returnVal = fc.showOpenDialog(AppInterface.this);
					
					if (returnVal == JFileChooser.APPROVE_OPTION) {
						parser.parseLogFile(fc.getSelectedFile());
						parser.analyzeThreads();
						parser.createThreadEventtable();
						graphCanvas.setLogData(parser.getThreadList(), parser.getEntries());
						graphCanvas.setTimeScale(parser.getInitialTime(), parser.getFinalTime());
						graphCanvas.repaint();
					}
				}
			});
		}
		return miOpenLogFile;
	}

	/**
	 * This method initializes miExit	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiExit() {
		if (miExit == null) {
			miExit = new JMenuItem();
			miExit.setText("Exit");
			miExit.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					System.exit(0);
				}
			});
		}
		return miExit;
	}

	/**
	 * This method initializes pmPopUpMenu	
	 * 	
	 * @return javax.swing.JPopupMenu	
	 */
	private JPopupMenu getPmPopUpMenu() {
		if (pmPopUpMenu == null) {
			pmPopUpMenu = new JPopupMenu();
			pmPopUpMenu.add(getMiPUResetZoom());
		}
		return pmPopUpMenu;
	}

	/**
	 * This method initializes miPUResetZoom	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiPUResetZoom() {
		if (miPUResetZoom == null) {
			miPUResetZoom = new JMenuItem();
		}
		return miPUResetZoom;
	}

	/**
	 * This method initializes mTools	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getMTools() {
		if (mTools == null) {
			mTools = new JMenu();
			mTools.setText("Tools");
			mTools.add(getMiResetZoom());
		}
		return mTools;
	}

	/**
	 * This method initializes miResetZoom	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiResetZoom() {
		if (miResetZoom == null) {
			miResetZoom = new JMenuItem();
			miResetZoom.setText("Reset Zoom");
			miResetZoom.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					graphCanvas.setTimeScale(parser.getInitialTime(), parser.getFinalTime());
					graphCanvas.repaint();
				}
			});
		}
		return miResetZoom;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			//e.printStackTrace();
			System.out.println("Look and Feel Initialization Error");
		} catch (InstantiationException e) {
			System.out.println("Look and Feel Initialization Error");
		} catch (IllegalAccessException e) {
			System.out.println("Look and Feel Initialization Error");
		} catch (UnsupportedLookAndFeelException e) {
			// TODO Auto-generated catch block
			System.out.println("Look and Feel Initialization Error");
		}
		// TODO Auto-generated method stub
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				AppInterface thisClass = new AppInterface();
				thisClass.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
				thisClass.pack();
				thisClass.setVisible(true);
			}
		});
	}

	/**
	 * This is the default constructor
	 */
	public AppInterface() {
		super();
		initialize();
		parser = new LogParser();
		//display.initBasicLayout();
		//display.redraw();
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(908, 630);
		this.setJMenuBar(getMbOvLog());
		this.setLocation(200, 50);
		this.setContentPane(getJContentPane());
		this.setResizable(false);
		this.setTitle("OvLog - ObelISK Log Viewer");
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.setBounds(1, 0, 900, 600);
			jContentPane.setPreferredSize(new Dimension(900, 600));
			jContentPane.add(getJPanelDisplay(), null);
		}
		return jContentPane;
	}
}

