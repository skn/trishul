package trishul.test.trishul_p;

/**
 * Generic test driver for engine files.
 * All engine files are run against this source file. If the output does not match the output in
 * <engine>.out, an error is printed.
 */
class TestGeneric
{
/*	static class T1
	{
		String getString () {return "T1";}
	}

	static class T2 extends T1
	{
		String getString () {return "T2";}
	}
*/
	static int getInt ()
	{
		return 666;
	}

	static String getString ()
	{
		return "Orignal string";
	}

	static String passString (String s)
	{
		return s;
	}

	static int passInt (int i)
	{
		return i;
	}

	void testParams (int i, String s)
	{
		testNested (i, s);
	}

	void testNested (int i, String s)
	{
	}

	void testSecond (String s, int i)
	{
	}

	void test1 ()
	{
	}

	static void doubleParams (long p2, double p1)
	{
	}

	static void main (String[] args)
	{
		try
		{
			int i = passInt (getInt ()); System.out.println (i);
			if (i > 0) System.out.println (passString ("NOT ALLOWED"));
			System.out.println (getString ());
			TestGeneric t = new TestGeneric ();
			t.testParams (passInt (1), passString ("A"));
			t.testSecond (passString ("B"), passInt (2));
			t.test1 ();
			Test1 t1 = new Test3 ();
			t1.getString ();
			t1 = new Test1 ();
			t1.getString ();
			doubleParams (0x1234567890abcdefL, Double.MAX_VALUE);
		}
		catch (Exception e)
		{
			e.printStackTrace (System.out);
		}
	}
}
