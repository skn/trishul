package trishul.test.taint.elseleak;

class elseleak1
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
   		// Value to be leaked
    	boolean a = taint (false, 1);

		boolean b = false, c = false;
		boolean d = false; // Receives leaked value
		boolean e = false; // Not used

    	checkTaint ("context before a", getContextTaint (), 0);

		if (a) b = true;
		else c = true;

    	checkTaint ("context after a", getContextTaint (), 0);

		checkTaint ("b", b, 1);
		checkTaint ("c", c, 1);

		if (!b)
		{
	    	checkTaint ("context b", getContextTaint (), 1);
	    	checkTaint ("d before b", d, 0x01); // 1 because of context
			d = false;
	    	checkTaint ("d after b", d, 0x01); // 1 because of context
		}
		if (!c)
		{
	    	checkTaint ("context c", getContextTaint (), 1);
	    	checkTaint ("d before c", d, 0x01); // 1 because of context
			d = true;
	    	checkTaint ("d after c", d, 0x01); // 1 because of context
		}

    	checkTaint ("context taint", getContextTaint (), 0);
    	checkTaint ("context leak", d, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
