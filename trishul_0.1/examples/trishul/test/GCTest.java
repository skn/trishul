package trishul.test;

import java.io.*;
import java.lang.reflect.*;

/**
 * To run: ./kaffe-tst -Xtainter trishul.test.GCTest\$TestTainter trishul.test.GCTest
 */
class GCTest
{

	static void main (String args[])
	{
		try
		{
			StringBuffer sb = new StringBuffer ();
			for (int i = 0; i < 4 * 1024; i++)
			{
				sb.append ("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
			}

			System.out.println (sb.toString ());
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}

	static void doHook (int[] test, int j)
	{
	}


	static class TestTainter
	extends trishul.Tainter
	{
		TestTainter ()
		throws Exception
		{
			registerHook (GCTest.class, "doHook", new Class[] {int[].class, int.class},
						  null, null, "doHook");

		}

		void doHook (int[] buf, int j, Method _hooked)
		{
			buf[j] = taint (j, 1);
			taintArray (buf, 2);
		}

	}
}
