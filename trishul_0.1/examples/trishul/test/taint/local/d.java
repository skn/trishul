package trishul.test.taint.local;

class d
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	double local1 = taint ((double) 12334.6789, 0x07);
    	double ref1, ref2, ref3, ref4; // Cause index overflow
    	double local2 = taint ((double) 12334.6789, 0x09);
    	checkTaint ("dstore_n & dload_n", local1, 0x07);
    	checkTaint ("dstore & dload", local2, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
