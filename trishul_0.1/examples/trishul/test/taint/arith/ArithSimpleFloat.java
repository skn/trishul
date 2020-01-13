package trishul.test.taint.arith;

/**
 * Tests simple arithmatical float instructions, without context policies
 */
class ArithSimpleFloat
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	float i = taint ((float) 3.0, 0x01); checkTaint ("Init", i, 0x01);
    	float j = taint ((float) 4.0, 0x02); checkTaint ("Init", j, 0x02);
    	checkTaint ("FADD", i + j,   0x03);
    	checkTaint ("FDIV", i / j,   0x03);
    	checkTaint ("FMUL", i * j,   0x03);
    	checkTaint ("FREM", i % j,   0x03);
    	checkTaint ("FSUB", i - j,   0x03);
    	checkTaint ("FNEG ", -j,   	 0x02);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
