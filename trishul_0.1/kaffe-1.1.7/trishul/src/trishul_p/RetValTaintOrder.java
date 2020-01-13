package trishul_p;

/**
 *
 * TODO: what if the function exits with an exception?
 */
public class RetValTaintOrder
extends TaintOrder
{
	public RetValTaintOrder (Taint taint)
	{
		super (taint);
	}

	public int getType ()
	{
		return TYPE_TAINT_RETURN_VALUE;
	}
}
