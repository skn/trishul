package trishul_p;

/**
 * Indicates the system should throw the supplied exception rather than execute the action.
 */
public class ExceptionOrder
extends Order
{
	private Exception exception;

	public ExceptionOrder (Exception exception)
  	{
  		this.exception = exception;
  	}

  	public Exception getException ()
  	{
  		return exception;
  	}

	public int getType ()
	{
		return TYPE_EXCEPTION;
	}
}
