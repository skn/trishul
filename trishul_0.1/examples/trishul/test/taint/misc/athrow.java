package trishul.test.taint.misc;

class athrow
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	try
    	{
    		throw (Exception) taint (new Exception (), 0x87);
    	}
    	catch (Exception e)
    	{
    		checkTaint ("athrow", e, 0x87);
    	}

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
