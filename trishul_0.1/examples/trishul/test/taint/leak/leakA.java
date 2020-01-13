package trishul.test.taint.leak;

class leakA
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int i = taint (13, 1);
    	int b = -1;

    	do
    	{
    		b++;
    	} while (b < i);

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
