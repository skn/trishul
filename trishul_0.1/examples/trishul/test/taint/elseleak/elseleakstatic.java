package trishul.test.taint.elseleak;

class elseleakstatic
extends trishul.test.taint.TaintTest
{
	static boolean b = false, c = false;
	static boolean d = false; // Receives leaked value

    public static void main(String args[])
    {
    	// Value to be leaked
    	boolean a = taint (false, 1);


		if (a) b = true;
		else c = true;

		if (!b) d = false;
		if (!c) d = true;


    	checkTaint ("context leak", d, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
