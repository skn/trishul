package trishul.test.taint.cmp;

class i
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	/*int i1 = taint (1, 0x0F);
    	int i2 = taint (2, 0xF0);
    	int i3 = taint (2, 0x0F);
    	checkTaint ("cmp > false", i2 > i3, 0xFF);
    	checkTaint ("cmp > true", i2 > i1, 0xFF);
    	checkTaint ("cmp < false", i3 < i2, 0xFF);
    	checkTaint ("cmp < true", i1 < i2, 0xFF);
    	checkTaint ("cmp = false", i2 == i1, 0xFF);
    	checkTaint ("cmp = true", i2 == i3, 0xFF);*/

	   	int i2 = taint (2, 0xF0);
    	int i3 = taint (2, 0x0F);
    	boolean b = i3 < i2;
    	checkTaint ("cmp < false", b, 0xFF);
	}
}
