package trishul.test.taint.field;

class _static
extends trishul.test.taint.TaintTest
{
	private	static boolean	b;
	private	static short	s;
	private	static char		c;
	private	static int		i;
	private	static long		l;
	private	static double	d;
	private	static float	f;
	private	static Object	a;

    public static void main(String args[])
    {
    	_static.b = taint (true, 0x01);
    	checkTaint ("boolean",	_static.b, 0x01);

    	_static.s = taint ((short) 13, 0x02);
    	checkTaint ("short", _static.s, 0x02);

    	_static.c = taint ('a', 0x03);
    	checkTaint ("char",	_static.c, 0x03);

    	_static.i = taint (7, 0x04);
    	checkTaint ("int",	_static.i, 0x04);

    	_static.l = taint ((long) 874783, 0x05);
    	checkTaint ("long",	_static.l, 0x05);

    	_static.d = taint ((double)98.624, 0x06);
    	checkTaint ("double",	_static.d, 0x06);

    	_static.f = taint ((float) 8789.53345, 0x07);
    	checkTaint ("float",	_static.f, 0x07);

    	_static.a = taint (new String ("sss"), 0x08);
    	checkTaint ("Object",	_static.a, 0x08);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
