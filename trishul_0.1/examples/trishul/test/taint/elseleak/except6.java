package trishul.test.taint.elseleak;

class except6
extends trishul.test.taint.TaintTest
{
	static boolean b = true;
    public static void main(String args[])
    {
    	leak (taint (true, 0x13)); 	checkTaint ("true", b, 0x13);
    	b = true;
    	leak (taint (false, 0x92)); checkTaint ("false", b, 0x92);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}

	static void leak (boolean a)
	{
		try
		{
			leak1 (a);
			b = false;
		}
		catch (Exception e)
		{
		}
	}

	static void leak1 (boolean a)
	throws Exception
	{
		if (a) throw new Exception ();
	}

}