package trishul.test.taint.local;

class i
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int local1 = taint ((int) 12334, 0x07);
    	int ref1, ref2, ref3, ref4; // Cause index overflow
    	int local2 = taint ((int) 12334, 0x09);
    	checkTaint ("istore_n & iload_n", local1, 0x07);
    	checkTaint ("istore & iload", local2, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
