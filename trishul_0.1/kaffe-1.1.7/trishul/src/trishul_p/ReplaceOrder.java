package trishul_p;

/**
 * An order which replaces an action with an engine-defined return value.
 * The return value may be a boxed primitive; it will be unboxed when required.
 */
public class ReplaceOrder
extends Order
{
	private Object replaceValue;

	public ReplaceOrder (Object replaceValue)
  	{
  		this.replaceValue = replaceValue;
  	}

  	public Object getReplaceValue ()
  	{
  		return replaceValue;
  	}

	public int getType ()
	{
		return TYPE_REPLACE;
	}
}
