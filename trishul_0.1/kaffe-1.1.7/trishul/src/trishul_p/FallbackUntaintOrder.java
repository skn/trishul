package trishul_p;

/**
 *
 */
public class FallbackUntaintOrder
extends TaintOrder
{
	public FallbackUntaintOrder (Taint untaint)
	{
		super (untaint);
	}

	public int getType ()
	{
		return TYPE_UNTAINT_FALLBACK;
	}
}
