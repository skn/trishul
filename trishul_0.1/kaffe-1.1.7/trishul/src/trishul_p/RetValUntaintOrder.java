package trishul_p;

/**
 *
 */
public class RetValUntaintOrder
extends TaintOrder
{
	public RetValUntaintOrder (Taint untaint)
	{
		super (untaint);
	}

	public int getType ()
	{
		return TYPE_UNTAINT_RETURN_VALUE;
	}
}
