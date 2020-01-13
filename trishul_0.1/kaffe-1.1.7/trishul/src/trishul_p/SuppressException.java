package trishul_p;


/**
 * Exception raised when an action is suppressed using a SuppressOrder.
 */
public class SuppressException
extends TrishulPException
{
  	public SuppressException ()
  	{
	}

  	public SuppressException (String msg)
  	{
  		super (msg);
  	}
}
