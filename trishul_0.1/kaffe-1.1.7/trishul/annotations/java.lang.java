package java.lang;

trishultaint String
{
	notrishultaint int hash;

	public String (byte[] b, int offset, int len)
	{
		setObjectTaint (this, getArrayTaint (b));
		super (b, offset, len);
	}

	public int hashCode ()
	{
		int h = super ();
		taint (h, getObjectTaint (this));
		return h;
	}
}

/*
// Reimplemented as native
trishultaint System
{
	public static void arraycopy (Object src, int srcOff, Object dst, int dstOff, int len)
	{
		for (int i = 0; i < len; i++)
			taintArrayElement (dst, i + dstOff, getArrayElementTaint (src, i + srcOff));
		/* Cannot use super here, as that would require boxing of the int parameters,
		 * which will cause a cyclic dependency on System.arraycopy.
		 *
		java.lang.System.arraycopy (src, srcOff, dst, dstOff, len);
	}
}
*/