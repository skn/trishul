package trishul.test.taint.arith;

/**
 * Tests simple arithmatical integer instructions, without context policies
 */
class ArithSimpleInt
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int i = taint (3, 0x01); checkTaint ("Init", i, 0x01);
    	int j = taint (4, 0x02); checkTaint ("Init", j, 0x02);
    	checkTaint ("IADD ", i + j,   0x03);
    	checkTaint ("IAND ", i & j,   0x03);
    	checkTaint ("IDIV ", i / j,   0x03);
    	checkTaint ("IMUL ", i * j,   0x03);
    	checkTaint ("IOR  ", i | j,   0x03);
    	checkTaint ("IREM ", i % j,   0x03);
    	checkTaint ("ISHL ", i << j,  0x03);
    	checkTaint ("ISHR ", i >> j,  0x03);
    	checkTaint ("ISUB ", i - j,   0x03);
    	checkTaint ("IUSHR", i >>> j, 0x03);
    	checkTaint ("IXOR ", i ^ j,   0x03);
    	checkTaint ("INEG ", -j,   	  0x02);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
