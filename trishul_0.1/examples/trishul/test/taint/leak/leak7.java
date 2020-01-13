package trishul.test.taint.leak;

class leak7
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int a = taint (1, 1);
    	int b;
    	if (a == 0)
    	{
    		b = 0;
    	}
    	else if (a == 1)
    	{
    		b = 1;
    	}
    	else
    	{
    		b = 2;
    	}

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
