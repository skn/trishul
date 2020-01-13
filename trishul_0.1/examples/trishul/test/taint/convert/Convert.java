package trishul.test.taint.convert;

/**
 * Tests simple conversions, without context policies
 */
class Convert
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Conversions from double
    	double d = taint ((double) 3, 0x05); checkTaint ("Init", d, 0x05);
    	float d2f = (float) d; checkTaint ("D2F", d2f, 0x05);
    	int   d2i = (int)   d; checkTaint ("D2I", d2i, 0x05);
    	long  d2l = (long ) d; checkTaint ("D2L", d2l, 0x05);

    	// Conversions from float
    	float f = taint ((float) 3, 0x06); checkTaint ("Init", f, 0x06);
    	double f2d = (double) f; checkTaint ("F2D", f2d, 0x06);
    	int    f2i = (int)    f; checkTaint ("F2I", f2i, 0x06);
    	long   f2l = (long )  f; checkTaint ("F2L", f2l, 0x06);

    	// Conversions from long
    	long l = taint ((long) 3, 0x07); checkTaint ("Init", l, 0x07);
    	double l2d = (double) l; checkTaint ("L2D", l2d, 0x07);
    	int    l2i = (int)    l; checkTaint ("L2I", l2i, 0x07);
    	float  l2f = (float)  l; checkTaint ("L2F", l2f, 0x07);

    	// Conversions from int
    	int i = taint ((int) 3, 0x08); checkTaint ("Init", i, 0x08);
    	byte   i2b = (byte)   i; checkTaint ("I2B", i2b, 0x08);
    	char   i2c = (char)   i; checkTaint ("I2C", i2c, 0x08);
    	double i2d = (double) i; checkTaint ("I2D", i2d, 0x08);
    	float  i2f = (float)  i; checkTaint ("I2F", i2f, 0x08);
    	long   i2l = (long)   i; checkTaint ("I2L", i2l, 0x08);
    	short  i2s = (short)  i; checkTaint ("I2S", i2s, 0x08);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
