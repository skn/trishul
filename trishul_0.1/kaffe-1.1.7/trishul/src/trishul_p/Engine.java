// This is the superclass of all engines.
package trishul_p;

public abstract class Engine
extends trishul.Engine
implements ResultHandler
{
	private Object	securityManager;

	/**
	 *
	 */
	final protected native boolean __TRISHUL_MATCHES (TrishulActionPattern pattern, AbstractAction abs, Action action);

	final protected boolean __TRISHUL_MATCHES (TrishulActionPattern pattern, Action action)
	{
		return __TRISHUL_MATCHES (pattern, null, action);
	}

	protected class EngineHandle {}

	/**
	 *
	 * @param enginePolicy The engine's policy. null means a policy that allows everything.
	 */
	protected native EngineHandle addEngine (Class engineClass, java.security.Policy enginePolicy)
	throws SecurityException;

	protected EngineHandle addEngine (byte[] engineClass, java.security.Policy enginePolicy)
	throws SecurityException
	{
		return addEngine (engineClass, 0, engineClass.length, enginePolicy);
	}

	protected native EngineHandle addEngine (byte[] engineClass, int offset, int length,
														java.security.Policy enginePolicy)
	throws SecurityException;

	protected native void removeEngine (EngineHandle handle);

	protected native java.security.Policy loadEnginePolicy (java.net.URL enginePolicyURL)
	throws SecurityException;

	/**
	 * Returns an engine policy that allows nothing.
	 */
	protected native java.security.Policy getDisallowEnginePolicy ();


	/**
	 * Query the engine to see what it orders.
	 * NOTE: As a programming paradigm, this should be an effect-free
	 * method.  
	 */
	abstract protected Order query	(Action a);
	protected native Order doQuery (Action a);

	/**
	 * This method gets called to indicate that the Order returned
	 * from the query method has been accepted and is ABOUT TO BE executed.
	 */
	protected void accept (Order order)
	{
	}

	/**
	 * Invoked when the interface is registered with the order. The method will be invoked after
	 * the order is taken and the action proscribed by the order is executed.
	 *
	 * @param action 		The action originally passed to the query method
	 * @param order 	The order originally returned by query
	 * @param result		The action's result. If the action returned a primitive, it will have been boxed.
	 * @param isException	If true, the action caused an exception. The exception is passed in result.
	 */
	public void handleResult (Action action, Order order, Object result, boolean isException)
	{
	}
}
