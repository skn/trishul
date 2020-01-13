package trishul.test.taint.elseleak;

class fallback
extends trishul.test.taint.TaintTest
{
	boolean d;

	fallback (boolean a)
	{
		boolean b = false, c = false;

		d = this == null;

		if (a) b = true;
		else c = true;

		if (!b) d = false;
		if (!c) d = true;

    	checkTaint ("fallback", getFallbackTaint (), 1);
	}

    public static void main(String args[])
    {
    	checkTaint ("context leak", new fallback (taint (false, 1)).d, 0x01);
	}
}
