package trishul.test.taint.returns;

/**
 * Tests return of longs
 */
class ReturnLong
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	long i = staticReturn (); checkTaint ("Static return", i, 0x07);
    	long j = new ReturnLong ().dynamicReturn (); checkTaint ("Dynamic return", j, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }

    public static long staticReturn ()
    {
		return taint ((long) 1, 0x07);
    }

    public long dynamicReturn ()
    {
		return taint ((long) 2, 0x09);
    }

}
