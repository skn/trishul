package trishul.test.taint.elseleak;

class except5
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	checkTaint ("true",		leak (taint (true, 0x13)), 0x13);
    	checkTaint ("false",	leak (taint (false, 0x92)), 0x92);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}

	static boolean leak (boolean a)
	{
		boolean b = true;
		try
		{
			leak1 (a);
			b = false;
		}
		catch (Exception e)
		{
		}
		return b;
	}

	static void leak1 (boolean a)
	throws Exception
	{
		if (a) throw new Exception ();
	}

}