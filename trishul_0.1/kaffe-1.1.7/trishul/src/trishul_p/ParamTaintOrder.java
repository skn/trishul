package trishul_p;

public class ParamTaintOrder
extends TaintOrder
{
	private int parameterIndex;

	public ParamTaintOrder (int parameterIndex, Taint taint)
	{
		super (taint);
		this.parameterIndex	= parameterIndex;
	}

	public int getType ()
	{
		return TYPE_TAINT_PARAMETER;
	}

	public int getParameterIndex ()
	{
		return parameterIndex;
	}
}
