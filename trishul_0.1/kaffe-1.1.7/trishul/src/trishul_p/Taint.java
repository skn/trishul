package trishul_p;

/**
 * A taint, which can be used to apply a taint, or test for one.
 */
public class Taint
{
	/**
	 * The taint is applied to a primitive value.
	 * If the tainted entity is a (reference to an) object, the reference is tainted.
	 */
	public static final int 	TAINT_PRIMITIVE = 0;
	public static final int 	TAINT_OBJECT	= 1;

	/**
	 * TAINT_OBJECT if the tainted value is an object, TAINT_PRIMITIVE otherwise.
	 */
	public static final int 	TAINT_AUTO		= 2;

	public static final int 	MAX_TAINT = Integer.MIN_VALUE;

	protected int type;
	protected int taint;

	/**
	 * Constructor.
	 */
	public Taint (int type, int taint)
	{
		this.type	= type;
		this.taint	= taint;
	}


	/**
	 * Retrieves the type of taint to be applied.
	 * @return TAINT_PRIMITIVE
	 * @return TAINT_OBJECT
	 * @return TAINT_AUTO
	 */
	public int getType ()
	{
		return type;
	}

	/**
	 * Retrieves the taint value.
	 */
	public int getTaint ()
	{
		return taint;
	}

	protected static final String TYPE_STRINGS[] =
	{
		"primitive", "object", "auto"
	};

	public String toString ()
	{
		return TYPE_STRINGS[type] + ":" + taint;
	}
}
