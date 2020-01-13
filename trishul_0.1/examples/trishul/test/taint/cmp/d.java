package trishul.test.taint.cmp;

class d
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	double l = taint ((double) 1, 0x0F);
    	double r = taint ((double) 2, 0xF0);
    	checkTaint ("dcmpl", l > r, 0xFF);
    	checkTaint ("dcmpg", l < r, 0xFF);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
