package trishul.test.taint;

public class TaintTest
{
	/**
	 * Returns the specified value, tainted with the
	 * specified taint.
	 */
	static protected native int 	taint (int value, int taint);
	static protected native long 	taint (long value, int taint);
	static protected native double 	taint (double value, int taint);
	static protected native float 	taint (float value, int taint);
	static protected native char 	taint (char value, int taint);
	static protected native short 	taint (short value, int taint);
	static protected native byte 	taint (byte value, int taint);
	static protected native boolean taint (boolean value, int taint);
	static protected native Object 	taint (Object value, int taint);
	static protected native void	taintObject (Object object, int taint);
	static protected native void	taintArray  (Object array, int taint);

	static protected native int		getTaint (int value);
	static protected native int		getTaint (long value);
	static protected native int		getTaint (double value);
	static protected native int		getTaint (float value);
	static protected native int		getTaint (Object value);
	static protected native int		getTaint (byte value);
	static protected native int		getTaint (short value);
	static protected native int		getTaint (boolean value);
	static protected native int		getTaint (char value);
	static protected native int		getObjectTaint 	(Object ref);
	static protected native int		getArrayTaint  	(Object ref);
	static protected native int		getFallbackTaint();
	static protected native int		getContextTaint ();

	/**
	 * Checks if value has the specified taint. If not, an error is printed.
	 */
	static protected native void checkTaint (String desc, int value, int wantTaint);
	static protected native void checkTaint (String desc, long value, int wantTaint);
	static protected native void checkTaint (String desc, double value, int wantTaint);
	static protected native void checkTaint (String desc, float value, int wantTaint);
	static protected native void checkTaint (String desc, char value, int wantTaint);
	static protected native void checkTaint (String desc, short value, int wantTaint);
	static protected native void checkTaint (String desc, byte value, int wantTaint);
	static protected native void checkTaint (String desc, boolean value, int wantTaint);
	static protected native void checkTaint (String desc, Object value, int wantTaint);
	static protected native void checkObjectTaint (String desc, Object value, int wantTaint);
	static protected native void checkArrayTaint (String desc, Object value, int wantTaint);

	static protected native boolean getConfigOption (String option);
}
