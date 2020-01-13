package trishul.test.taint.returns;

/**
 * Tests return of integers
 */
class ReturnInt
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int i = staticReturn (); checkTaint ("Static return", i, 0x07);
    	int j = new ReturnInt ().dynamicReturn (); checkTaint ("Dynamic return", j, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }

    public static int staticReturn ()
    {
		return taint (1, 0x07);
    }

    public int dynamicReturn ()
    {
		return taint (2, 0x09);
    }

}
