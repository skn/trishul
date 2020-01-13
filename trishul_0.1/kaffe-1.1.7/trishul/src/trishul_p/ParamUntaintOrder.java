package trishul_p;

public class ParamUntaintOrder
extends TaintOrder
{
	private int parameterIndex;

	public ParamUntaintOrder (int parameterIndex, Taint taint)
	{
		super (taint);
		this.parameterIndex	= parameterIndex;
	}

	public int getType ()
	{
		return TYPE_UNTAINT_PARAMETER;
	}

	public int getParameterIndex ()
	{
		return parameterIndex;
	}
}
