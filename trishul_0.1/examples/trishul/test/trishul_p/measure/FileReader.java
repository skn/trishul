package trishul.test.trishul_p.measure;

import java.io.*;
import java.lang.reflect.*;

/**
 * To run: ./kaffe-tst -Xengine trishul.test.trishul_p.measure.FileReaderEngine trishul.test.trishul_p.measure.FileReader
 */
class FileReader
{

	static void main (String args[])
	{
		long start_time = System.currentTimeMillis();
		long end_time;

		try
		{
			StringBuffer  contents = new StringBuffer();

			for (int i = 0; i < args.length; i++)
			{
				FileInputStream in     = new FileInputStream (args[i]);
				DataInputStream reader = new DataInputStream (in);

				while (reader.available() !=0)
				{
					String str = reader.readLine();
					contents.append(str);
					contents.append(System.getProperty("line.separator"));
				}
			}

			end_time = System.currentTimeMillis();
			System.out.print (contents.toString());
		}
		catch (Exception e)
		{
			end_time = System.currentTimeMillis();
			e.printStackTrace ();
		}

		System.err.println((end_time - start_time) + " ms");
	}
}
