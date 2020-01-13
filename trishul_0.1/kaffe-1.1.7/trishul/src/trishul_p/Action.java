package trishul_p;

public abstract class Action
extends Throwable
{
	public abstract Object getThisPointer ();

	public abstract int	   getParameterCount ();
	public abstract Object getActualParameter (int index);
	public abstract Class  getFormalParameter (int index);

	public String getContext ()
	{
		return getContext (0);
	}

	public String getContext (int depth)
	{
		StackTraceElement stack = getStackTrace ()[depth];
		return stack.getClassName () + "." + stack.getMethodName ();
	}
}

