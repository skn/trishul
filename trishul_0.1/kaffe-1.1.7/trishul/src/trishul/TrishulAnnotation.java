package trishul;

public class TrishulAnnotation
{
	protected final static int TAINT_ALL = 0xFFFFFFFF;

	protected final static native void taintObject 		(Object obj, int taint);

	/**
	 * @return An untainted reference to the untainted object.
	 */
	protected final static native <Type> Type untaintObject (Type obj, int untaint);
	protected final static native int  getObjectTaint	(Object obj);
	protected final static native void setObjectTaint	(Object obj, int taint);

	protected final static native void taintArray 		(Object array, int taint);
	protected final static native void untaintArray 	(Object array, int untaint);
	protected final static native int  getArrayTaint	(Object array);
	protected final static native void setArrayTaint	(Object obj, int taint);

	protected final static native void taintArrayElement 	(Object array, int index, int taint);
	protected final static native void untaintArrayElement 	(Object array, int index, int untaint);
	protected final static native int  getArrayElementTaint (Object array, int index);
	protected final static native void setArrayElementTaint (Object array, int index, int taint);

	protected final class This {};

	protected final static native <RetType> RetType $super (This $this, Object... arguments);
	protected final static native int 		$superI (This $this, Object... arguments);
	protected final static native long 		$superJ (This $this, Object... arguments);
	protected final static native double 	$superD (This $this, Object... arguments);
	protected final static native float 	$superF (This $this, Object... arguments);
	protected final static native char 		$superC (This $this, Object... arguments);
	protected final static native short 	$superS (This $this, Object... arguments);
	protected final static native byte 		$superB (This $this, Object... arguments);
	protected final static native boolean 	$superZ (This $this, Object... arguments);

	protected final static native <X extends Throwable> Object $superX (This $this, Object... arguments)
	throws X;

	protected final static native int 		taint (int value, int taint);
	protected final static native long 		taint (long value, int taint);
	protected final static native double 	taint (double value, int taint);
	protected final static native float 	taint (float value, int taint);
	protected final static native char 		taint (char value, int taint);
	protected final static native short 	taint (short value, int taint);
	protected final static native byte 		taint (byte value, int taint);
	protected final static native boolean 	taint (boolean value, int taint);
	protected final static native Object 	taint (Object value, int taint);

	protected final static native int		getTaint (int value);
	protected final static native int		getTaint (long value);
	protected final static native int		getTaint (double value);
	protected final static native int		getTaint (float value);
	protected final static native int		getTaint (Object value);
	protected final static native int		getTaint (byte value);
	protected final static native int		getTaint (short value);
	protected final static native int		getTaint (boolean value);
	protected final static native int		getTaint (char value);
	protected final static native int		getFallbackTaint ();
}
