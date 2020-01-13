package trishul.test.taint.returns;

/**
 * Tests return of floats
 */
class ReturnFloat
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	float i = staticReturn (); checkTaint ("Static return", i, 0x07);
    	float j = new ReturnFloat ().dynamicReturn (); checkTaint ("Dynamic return", j, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }

    public static float staticReturn ()
    {
		return taint ((float) 1, 0x07);
    }

    public float dynamicReturn ()
    {
		return taint ((float) 2, 0x09);
    }

}
