package trishul.test.taint.local;

class l
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	long local1 = taint ((long) 12334, 0x07);
    	long ref1, ref2, ref3, ref4; // Cause index overflow
    	long local2 = taint ((long) 12334, 0x09);
    	checkTaint ("lstore_n & lload_n", local1, 0x07);
    	checkTaint ("lstore & lload", local2, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
