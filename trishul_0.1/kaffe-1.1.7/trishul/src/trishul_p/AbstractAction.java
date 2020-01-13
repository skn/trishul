package trishul_p;

public abstract class AbstractAction
extends Action
{

	/* These trishul hook methods are intentionally not integrated with the Engine class, as this
	 * allows the this pointers to be checked
	 */
	final protected native boolean __TRISHUL_MATCHES 	(TrishulActionPattern pattern, Action action);


	/**
	 * Check whether parameter can be matched by this abstract action.
	 */
	public abstract boolean matches (Action a);
}
