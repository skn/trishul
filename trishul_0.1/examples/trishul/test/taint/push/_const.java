package trishul.test.taint.push;

class _const
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Check if taints are initialized to 0
    	checkTaint ("aconstnull", null, 0);
    	checkTaint ("dconst_0", (double) 0, 0);
    	checkTaint ("dconst_1", (double) 1, 0);
    	checkTaint ("fconst_0", (float) 0, 0);
    	checkTaint ("fconst_1", (float) 1, 0);
    	checkTaint ("fconst_2", (float) 2, 0);
    	checkTaint ("iconst_m1", (int) -1, 0);
    	checkTaint ("iconst_0", (int) 0, 0);
    	checkTaint ("iconst_1", (int) 1, 0);
    	checkTaint ("iconst_2", (int) 2, 0);
    	checkTaint ("iconst_3", (int) 3, 0);
    	checkTaint ("iconst_4", (int) 4, 0);
    	checkTaint ("iconst_5", (int) 5, 0);
    	checkTaint ("lconst_0", (long) 0, 0);
    	checkTaint ("lconst_1", (long) 1, 0);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
