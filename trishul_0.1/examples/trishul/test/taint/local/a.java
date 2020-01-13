package trishul.test.taint.local;

class a
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	Object local1 = taint (new String ("Test"), 0x07);
    	Object ref1, ref2, ref3, ref4; // Cause index overflow
    	Object local2 = taint (new String ("Test"), 0x09);
    	checkTaint ("astore_n & aload_n", local1, 0x07);
    	checkTaint ("astore & aload", local2, 0x09);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
