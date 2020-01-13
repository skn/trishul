package trishul.test.taint.arith;

/**
 * Tests simple arithmatical long instructions, without context policies
 */
class ArithSimpleLong
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	long i = taint ((long) 3, 0x01); checkTaint ("Init", i, 0x01);
    	long j = taint ((long) 4, 0x02); checkTaint ("Init", j, 0x02);
    	checkTaint ("LADD ", i + j,   0x03);
    	checkTaint ("LAND ", i & j,   0x03);
    	checkTaint ("LDIV ", i / j,   0x03);
    	checkTaint ("LMUL ", i * j,   0x03);
    	checkTaint ("LOR  ", i | j,   0x03);
    	checkTaint ("LREM ", i % j,   0x03);
    	checkTaint ("LSUB ", i - j,   0x03);
    	checkTaint ("LXOR ", i ^ j,   0x03);
    	checkTaint ("LNEG ", -j,   	  0x02);

    	// The shift operators are mixed operands
    	int k = taint (4, 0x02); checkTaint ("Init", k, 0x02);
    	checkTaint ("LSHL ", i << k,   0x03);
    	checkTaint ("LSHR ", i >> k,   0x03);
    	checkTaint ("LUSHR", i >>> k,  0x03);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
