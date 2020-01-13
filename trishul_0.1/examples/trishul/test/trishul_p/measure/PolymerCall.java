package trishul.test.trishul_p.measure;

class PolymerCall
{

	static void main (String args[])
	{
		long start_time = System.currentTimeMillis();
		long end_time;

		for (int i = 0; i < 10000; i++)
		{
			call2 (call1 (i));
		}

		end_time = System.currentTimeMillis();
		System.err.println(10*(end_time - start_time));
	}

	static int call1 (int i)
	{
		return i;
	}

	static void call2 (int i)
	{
	}

	static void callNever ()
	{
	}
}
