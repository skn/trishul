package trishul.test.gtk;
import java.awt.*;
import java.awt.event.*;

class Mouse
extends trishul.test.taint.TaintTest
implements MouseListener
{

	public static void main (String[] args)
	{
		try
		{
			Mouse m = new Mouse ();
			Robot r = new Robot ();
			r.mouseMove (100, 100);
			r.mousePress (taint (InputEvent.BUTTON2_MASK, 1));
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}

	Frame f;

	Mouse ()
	{
 		f = new Frame("Mouse");
 		f.addWindowListener (new WindowAdapter ()
        {
        	public void windowClosing (WindowEvent e)
        	{
        		System.exit (0);
        	}
        });
        f.setBounds (0, 0, 250, 250);
        f.setLayout (new BorderLayout ());
        f.addMouseListener (this);
        f.setVisible(true);
    }

	public void mousePressed (MouseEvent e)
	{
		System.out.println (e.getButton ());
		checkTaint ("Btn", e.getButton (), 1);
		System.exit (0);
	}

	public void mouseReleased (MouseEvent e){}
	public void mouseClicked (MouseEvent e){}
	public void mouseEntered (MouseEvent e){}
	public void mouseExited  (MouseEvent e){}
}