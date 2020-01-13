package trishul.test.gtk;
import java.awt.*;
import java.awt.event.*;

class HelloWorld
{
	public static void main (String[] args)
	{
 		Frame f = new Frame("Hello world");
        f.setSize(250,100);
        f.addWindowListener (new WindowAdapter ()
        {
        	public void windowClosing (WindowEvent e)
        	{
        		System.exit (0);
        	}
        });
        f.setVisible(true);
	}
};