package trishul.test.taint.cmp;

class l
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	long l1 = taint ((long) 1, 0x0F);
    	long l2 = taint ((long) 2, 0xF0);
    	long l3 = taint ((long) 2, 0x0F);
    	checkTaint ("lcmp >", l2 > l1, 0xFF);
    	checkTaint ("lcmp <", l1 < l2, 0xFF);
    	checkTaint ("lcmp =", l2 == l3, 0xFF);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
