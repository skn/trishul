package trishul.test.taint.field;

/**
 * Tests if object taint is include in the field taint.
 */
class object
extends trishul.test.taint.TaintTest
{
	private	boolean	b;
	private	short	s;
	private	char	c;
	private	int		i;
	private	long	l;
	private	double	d;
	private	float	f;
	private	Object	a;

    public static void main(String args[])
    {
    	object obj = new object ();

    	taintObject (obj, 0x20);

    	obj.b = taint (true, 0x01);
    	checkTaint ("boolean",	obj.b, 0x01);

    	obj.s = taint ((short) 13, 0x02);
    	checkTaint ("short",	obj.s, 0x02);

    	obj.c = taint ('a', 0x03);
    	checkTaint ("char",	obj.c, 0x03);

    	obj.i = taint (7, 0x04);
    	checkTaint ("int",	obj.i, 0x04);

    	obj.l = taint ((long) 874783, 0x05);
    	checkTaint ("long",	obj.l, 0x05);

    	obj.d = taint ((double)98.624, 0x06);
    	checkTaint ("double",	obj.d, 0x06);

    	obj.f = taint ((float) 8789.53345, 0x07);
    	checkTaint ("float",	obj.f, 0x07);

    	obj.a = taint (new String ("sss"), 0x08);
    	checkTaint ("Object",	obj.a, 0x08);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
