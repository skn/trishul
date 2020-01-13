package trishul.test.taint.elseleak;

class except2
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
			return leak2 (a);
		}
		catch (Exception e)
		{
			return true;
		}
	}

	static boolean leak2 (boolean a)
	throws Exception
	{
		if (a) throw new Exception ();
		return false;
	}
}