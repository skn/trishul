package trishul.test.taint.leak;

class leak4
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Leaks i into b and c
    	int i = taint (13, 1);
    	int b = 0;
    	int c = 0;

    	while (true)
    	{
    		b++;
    		if (b == i)
    		{
    			break;
    		}
    		c = b;
    	}

    	checkTaint ("context leak", b, 0x01);
    	checkTaint ("context leak", c, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
