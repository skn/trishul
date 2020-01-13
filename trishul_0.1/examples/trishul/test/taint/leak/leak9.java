package trishul.test.taint.leak;

class leak9
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	boolean a = taint (true, 1);
    	boolean b = false;
    	if (a)
    	{
    		b = true;
    	}

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
