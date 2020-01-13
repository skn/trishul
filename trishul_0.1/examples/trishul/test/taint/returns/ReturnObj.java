package trishul.test.taint.returns;

/**
 * Tests return of references
 */
class ReturnObj
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	Object i = staticReturn (); checkTaint ("Static return", i, 0x07);
    	Object j = new ReturnObj ().dynamicReturn (); checkTaint ("Dynamic return", j, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }

    public static Object staticReturn ()
    {
		return taint (new String (), 0x07);
    }

    public Object dynamicReturn ()
    {
		return taint (new String (), 0x09);
    }

}
