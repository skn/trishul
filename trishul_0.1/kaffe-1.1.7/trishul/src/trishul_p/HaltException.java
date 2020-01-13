package trishul_p;

/**
 * The exception that is thrown when a program is terminated through a HaltOrder.
 */
public class HaltException
extends TrishulPException
{
	public HaltException ()
	{
	}

	public HaltException (String msg)
	{
		super (msg);
	}
}


