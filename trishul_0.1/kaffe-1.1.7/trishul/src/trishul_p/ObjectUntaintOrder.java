package trishul_p;

/**
 *
 */
public class ObjectUntaintOrder
extends TaintOrder
{
	private Object	object;

	public ObjectUntaintOrder (Object object, Taint taint)
	{
		super (taint);
		if (object == null)
			throw new NullPointerException ();
		this.object	= object;
	}

	public int getType ()
	{
		return TYPE_UNTAINT_OBJECT;
	}

	public Object getObject ()
	{
		return object;
	}
}
