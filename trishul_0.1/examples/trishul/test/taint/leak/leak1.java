package trishul.test.taint.leak;

class leak1
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	boolean a = taint (true, 1);
    	boolean b;
    	if (a)
    	{
    		b = true;
    	}
    	else
    	{
    		b = false;
    	}

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
