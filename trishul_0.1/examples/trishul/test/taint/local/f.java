package trishul.test.taint.local;

class f
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	float local1 = taint ((float) 12334.6789, 0x07);
    	float ref1, ref2, ref3, ref4; // Cause index overflow
    	float local2 = taint ((float) 12334.6789, 0x09);
    	checkTaint ("fstore_n & fload_n", local1, 0x07);
    	checkTaint ("fstore & fload", local2, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
