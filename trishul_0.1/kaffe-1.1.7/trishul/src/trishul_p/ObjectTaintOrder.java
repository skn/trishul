package trishul_p;

/**
 *
 */
public class ObjectTaintOrder
extends TaintOrder
{
	private Object	object;

	public ObjectTaintOrder (Object object, Taint taint)
	{
		super (taint);
		if (object == null)
			throw new NullPointerException ();
		this.object	= object;
	}

	public int getType ()
	{
		return TYPE_TAINT_OBJECT;
	}

	public Object getObject ()
	{
		return object;
	}
}
