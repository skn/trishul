package trishul.test.taint.leak;

class leak3
extends trishul.test.taint.TaintTest
{
    static int b = 0;

	static void doleak (int i)
	{

    	while (true)
    	{
    		if (b == i)
    		{
    			return;
    		}
    		b++;
    	}
	}

    public static void main(String args[])
    {
    	int i = taint (13, 1);
    	doleak (i);
    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
