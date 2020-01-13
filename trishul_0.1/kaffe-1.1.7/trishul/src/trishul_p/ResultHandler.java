package trishul_p;

public interface ResultHandler
{
	/**
	 * Invoked when the interface is registered with the order. The method will be invoked after
	 * the order is taken and the action proscribed by the order is executed.
	 *
	 * @param action 		The action originally passed to the query method
	 * @param order 		The order originally returned by query
	 * @param result		The action's result. If the action returned a primitive, it will have been boxed.
	 * @param isException	If true, the action caused an exception. The exception is passed in result.
	 */
	public void handleResult (Action action, Order order, Object result, boolean isException);
}
