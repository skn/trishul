package trishul.test.taint.cmp;

class f
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	float l = taint ((float) 1, 0x0F);
    	float r = taint ((float) 2, 0xF0);
    	checkTaint ("fcmpl", l > r, 0xFF);
    	checkTaint ("fcmpg", l < r, 0xFF);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
