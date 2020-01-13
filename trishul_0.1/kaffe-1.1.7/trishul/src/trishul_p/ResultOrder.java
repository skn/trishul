
package trishul_p;

public abstract class ResultOrder
extends Order
{
	protected ResultHandler handler;

	protected ResultOrder (ResultHandler handler)
	{
		this.handler = handler;
	}

	public void setResultHandler (ResultHandler handler)
	{
		this.handler = handler;
	}

	public ResultHandler getResultHandler ()
	{
		return handler;
	}

	/**
	 * Invoked by kaffe to dispatch the handleResult call.
	 */
	private void handleResult (Object result, boolean isException)
	{
		if (handler != null)
		{
			handler.handleResult (triggerAction, this, result, isException);
		}
	}

}
