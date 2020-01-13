package trishul.test.taint.elseleak;

class fallbackExcept
extends trishul.test.taint.TaintTest
{
	boolean d;

	fallbackExcept (boolean a)
	{
		boolean b = false, c = false;

		d = this == null;

		if (a) b = true;
		else c = true;

		if (!b) d = false;
		if (!c) d = true;
	}

    public static void main(String args[])
    {
    	checkTaint ("context leak", new fallbackExcept (taint (false, 1)).d, 0x01);
	}
}
