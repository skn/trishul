package trishul.test.taint.returns;

/**
 * Tests return of doubles
 */
class ReturnDouble
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	double i = staticReturn (); checkTaint ("Static return", i, 0x07);
    	double j = new ReturnDouble ().dynamicReturn (); checkTaint ("Dynamic return", j, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }

    public static double staticReturn ()
    {
		return taint ((double) 1, 0x07);
    }

    public double dynamicReturn ()
    {
		return taint ((double) 2, 0x09);
    }

}
