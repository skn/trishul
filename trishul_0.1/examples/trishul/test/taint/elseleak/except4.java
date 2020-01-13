package trishul.test.taint.elseleak;

class except4
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
		try
		{
			return leak1 (a);
		}
		catch (Exception e)
		{
	    	checkTaint ("catch context", getContextTaint (), 0x92);
			return false;
		}
	}


	static boolean leak1 (boolean a)
	throws Exception
	{
		try
		{
			leak2 (a);
		}
		catch (RuntimeException e)
		{
	    	checkTaint ("catch context 1", getContextTaint (), 0x13);
		}

    	checkTaint ("non-catch context 1", getContextTaint (), 0x13);
		return true;
	}

	static boolean leak2 (boolean a)
	throws Exception
	{
		if (a) throw new RuntimeException ();
		throw new Exception ();
	}
}