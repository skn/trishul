package trishul.test.taint.elseleak;

class elseleak2
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Value to be leaked
    	boolean a = taint (false, 1);

		boolean b = false, c = false;
		boolean d = false; // Receives leaked value

		if (!a) c = true;
		if (!c) b = true;

		d = b;

    	checkTaint ("context leak", d, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
