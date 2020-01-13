package trishul.test.taint.elseleak;

class except0
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	String s = "";
    	checkTaint ("true",		leak (taint (true, 0x13)), 0x13);
    	checkTaint ("false",	leak (taint (false, 0x92)), 0x92);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}

	static boolean leak (boolean a)
	{
		boolean b;
		try
		{
			if (a) throw new Exception ();
			b = false;
		}
		catch (Exception e)
		{
			b = true;
		}
		return b;
	}
}