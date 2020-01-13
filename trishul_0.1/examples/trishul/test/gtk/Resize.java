package trishul.test.gtk;
import java.awt.*;
import java.awt.event.*;

class Resize
extends trishul.test.taint.TaintTest
implements ComponentListener
{

	public static void main (String[] args)
	{
		new Resize ().go ();
	}

	Frame f;
	Label l;
	Resize ()
	{
 		f = new Frame("Hello world");
        f.setSize(250,100);
        f.setLayout (new BorderLayout ());
        f.addWindowListener (new WindowAdapter ()
        {
        	public void windowClosing (WindowEvent e)
        	{
        		System.exit (0);
        	}
        });
        l = new Label ("Test");
        f.add (BorderLayout.CENTER, l);
 		l.addComponentListener (this);
 		f.addComponentListener (this);
        f.setVisible(true);
    }

    void go ()
    {

        f.setBounds (0, 0, taint (500, 0x07), 100);

        /*checkTaint ("-this", this, 0);
        checkObjectTaint ("-this object", this, 0);
        checkTaint ("-f", f, 0);
        checkObjectTaint ("-f object", f, 0);
        checkTaint ("-l", l, 0);
        checkObjectTaint ("-l object", l, 0);*/
    }

	public void componentResized(ComponentEvent e)
    {
        /*checkTaint ("this", this, 0);
        checkObjectTaint ("this object", this, 0);
        checkTaint ("f", f, 0);
        checkObjectTaint ("f object", f, 0);
        checkTaint ("l", l, 0);
        checkObjectTaint ("l object", l, 0);*/

		int i1 = e.paramString ().indexOf ("width=");
		int i2 = e.paramString ().indexOf (",height=");
		String s = e.paramString ().substring (i1 + 6, i2);
		int i = Integer.parseInt (s);
		System.out.println ("RESIZE: " + f.getWidth () + " = " + l.getWidth () + " - " + i);

        checkTaint ("String Width", i, 0x07);
        checkTaint ("Frame Width", f.getWidth (), 0x07);
        checkTaint ("Frame Height", f.getHeight () , 0x00);
        checkTaint ("Label Width", l.getWidth (), 0x07);

        //throw new IllegalArgumentException ();
	}

	public void componentMoved(ComponentEvent e){}
	public void componentShown(ComponentEvent e){}
	public void componentHidden(ComponentEvent e){}
}