package trishul.test.taint.push;

class push
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Check if taints are initialized to 0
		checkTaint ("bipush", (byte) 87, 0);
		checkTaint ("sipush", (short) 87, 0);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
