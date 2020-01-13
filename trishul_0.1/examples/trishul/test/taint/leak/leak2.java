package trishul.test.taint.leak;

class leak2
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int i = taint (13, 1);
    	int b = 0;

    	while (true)
    	{
    		if (b == i)
    		{
    			break;
    		}
    		b++;
    	}

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
