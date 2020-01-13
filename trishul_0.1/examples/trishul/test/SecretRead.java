package trishul.test;

import java.io.*;
import java.lang.reflect.*;

/**
 * To run: ./kaffe-tst -Xtainter trishul.test.SecretRead\$TestTainter trishul.test.SecretRead
 */
class SecretRead
{

	static void main (String args[])
	{
		try
		{
			FileInputStream in     = new FileInputStream (args[0]);
			DataInputStream reader = new DataInputStream (in);
			System.out.println (reader.readLine ());
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}

	static class TestTainter
	extends trishul.Tainter
	{
		TestTainter ()
		throws Exception
		{
			registerHook (FileInputStream.class, null, new Class[] {String.class},
						  null, null, "FileInputStream_ctor");

			registerHook (FileInputStream.class, "read", new Class[] {byte[].class, int.class, int.class},
						  null, null, "FileInputStream_read");

			registerHook (FileInputStream.class, "read", new Class[] {},
						  null, null, "FileInputStream_read_I");

			registerHook (PrintStream.class, "println", new Class[] {String.class},
						  System.out, null, "System_out_println");

		}

		void FileInputStream_ctor (String path, Constructor _hooked, FileInputStream _this)
		{
			if (path.indexOf ("secret") >= 0) taintObject (_this, 0x01);
			invokeConstructor (_hooked, _this, new Object[] {path});
			//System.err.println ("*********** TAINT1: " + _this + ", " + getObjectTaint (_this));
		}

		int FileInputStream_read (byte[] buf, int offset, int length, Method _hooked, FileInputStream _this)
		{
			Object o = invokeMethod (_hooked, _this, new Object[] {buf, new Integer (offset), new Integer (length)});
			taintArray (buf, getObjectTaint (_this));
			return ((Integer) o).intValue ();
		}

		int FileInputStream_read_I (Method _hooked, FileInputStream _this)
		{
			Object o = invokeMethod (_hooked, _this, new Object[] {});
			int ret = taint (((Integer) o).intValue (), getObjectTaint (_this));
			//System.err.println ("*********** TAINT2: " + _this + ", " + getTaint (ret));
			return ret;
		}

		void System_out_println (String text, Method _hooked, PrintStream _this)
		throws Exception
		{
			//System.err.println ("****** Check taint");
			if (getObjectTaint (text) != 0 || getTaint (text.charAt (0)) != 0)
			{
				// Tainted, error
				//System.err.println ("****** Disallowing tainted string to System.out.println");
				throw new trishul.SecurityException ("Disallowing tainted string to System.out.println");
			}
			else
			{
				// Ok, execute println
				//System.err.println ("****** Ok, execute println");
				invokeMethod (_hooked, _this, new Object[] {text});
			}
		}

	}
}