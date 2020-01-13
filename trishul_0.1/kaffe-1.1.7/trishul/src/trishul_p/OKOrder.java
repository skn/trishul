package trishul_p;

public class OKOrder
extends ResultOrder
{
	public OKOrder ()
  	{
  		super (null);
  	}

	public OKOrder (ResultHandler handler)
  	{
  		super (handler);
  	}

	public int getType ()
	{
		return TYPE_OK;
	}
}

