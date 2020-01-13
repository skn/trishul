package trishul.test.taint.elseleak;

class elseleakfield
extends trishul.test.taint.TaintTest
{
	boolean b = false, c = false;
	boolean d = false; // Receives leaked value

	elseleakfield (boolean a)
	{
		checkTaint ("param", a, 1);

		if (a) b = true;
		else c = true;

    	checkTaint ("b1", b, 0x01);
    	checkTaint ("c1", c, 0x01);

		if (!b) d = false;
		if (!c) d = true;
	}

    public static void main(String args[])
    {
    	elseleakfield elf = new elseleakfield (taint (false, 1));
    	checkTaint ("context leak", elf.d, 0x01);
    	checkTaint ("b", elf.b, 0x01);
    	checkTaint ("c", elf.c, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
