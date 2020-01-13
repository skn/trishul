package trishul.test.trishul_p.measure;

import java.io.*;
import java.lang.reflect.*;

/**
 * To run: ./kaffe-tst -Xengine trishul.test.trishul_p.measure.FileReaderEngine trishul.test.trishul_p.measure.FileReader
 */
class FileReader3
{

	static void main (String args[])
	{
		long start_time = System.currentTimeMillis();
		long end_time;

		try
		{
			doMain (args);
			end_time = System.currentTimeMillis();
		}
		catch (Exception e)
		{
			end_time = System.currentTimeMillis();
			e.printStackTrace ();
		}

		System.err.println((end_time - start_time) + " ms");
	}

	static void doMain (String[] args)
	throws Exception
	{
		byte[] buffer = new byte[1024];

		for (int i = 0; i < args.length; i++)
		{
			FileInputStream in     = new FileInputStream (args[i]);

			while (in.available() !=0)
			{
				int count = in.read (buffer);
				System.out.write (buffer, 0, count);
			}
		}
	}
}
