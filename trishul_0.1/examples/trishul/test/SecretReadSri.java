package trishul.test;

import java.io.*;
import java.lang.reflect.*;

/**
 * To run: ./kaffe-tst -Xtainter trishul.test.SecretReadSri\$TestTainter trishul.test.SecretReadSri
 */
class SecretReadSri
{

	static void main (String args[])
	{
		long start_time = System.currentTimeMillis();
		long end_time;
		try
		{
			FileInputStream in     = new FileInputStream (args[0]);
			DataInputStream reader = new DataInputStream (in);
			StringBuffer  contents = new StringBuffer();

			while (reader.available() !=0)
			{
				String str = reader.readLine();
				contents.append(str);
				contents.append(System.getProperty("line.separator"));
			}

			end_time = System.currentTimeMillis();
			String s = contents.toString();
			System.out.println (s);

		}
		catch (Exception e)
		{
			end_time = System.currentTimeMillis();
			e.printStackTrace ();
		}

		System.err.println("Time taken = " + (end_time - start_time) + "ms");
	}

	static class TestTainter
	extends trishul.Tainter
	{
		TestTainter ()
		throws Exception
		{
			registerHook (FileInputStream.class, null, new Class[] {String.class},
						  null, null, "FileInputStream_ctor");

			//registerHook (FileInputStream.class, "read", new Class[] {byte[].class, int.class, int.class},
			//			  null, null, "FileInputStream_read");

			//registerHook (FileInputStream.class, "read", new Class[] {},
			//			  null, null, "FileInputStream_read_I");

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
			//System.err.println ("*********** TAINT2: " + _this + ", " + getObjectTaint (_this) + " - " + o);
			return ((Integer) o).intValue ();
		}

		int FileInputStream_read_I (Method _hooked, FileInputStream _this)
		{
			Object o = invokeMethod (_hooked, _this, null);
			int ret = taint (((Integer) o).intValue (), getObjectTaint (_this));
			return ret;
		}

		void System_out_println (String text, Method _hooked, PrintStream _this)
		throws Exception
		{
			//System.err.println ("****** Check taint: " + getObjectTaint (text) + " - " + getTaint (text.charAt (0)));
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
