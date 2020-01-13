package trishul.test.taint.arith;

/**
 * Tests simple arithmatical double instructions, without context policies
 */
class ArithSimpleDouble
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	double i = taint (3.0, 0x01); checkTaint ("Init", i, 0x01);
    	double j = taint (4.0, 0x02); checkTaint ("Init", j, 0x02);
    	checkTaint ("DADD", i + j,   0x03);
    	checkTaint ("DDIV", i / j,   0x03);
    	checkTaint ("DMUL", i * j,   0x03);
    	checkTaint ("DREM", i % j,   0x03);
    	checkTaint ("DSUB", i - j,   0x03);
    	checkTaint ("DNEG ", -j,   	 0x02);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
