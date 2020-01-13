package trishul_p;

/**
 * The action that signals the termination of the program.
 */
public class DoneAction
extends Action
{
  	private Throwable terminationCause;

	public DoneAction (Throwable terminationCause)
	{
		this.terminationCause = terminationCause;
	}

	public boolean terminateAbnormally ()
	{
		return (terminationCause != null);
	}

  	public Throwable getAbnormalTerminationCause ()
  	{
  		return terminationCause;
  	}

  	public boolean equals (Object rhs)
  	{
  		if (!(rhs instanceof DoneAction))
  			return false;
  		return true;
  	}

  	public String toString()
  	{
  		return "done action";
  	}

	public Object getThisPointer ()
	{
		return null;
	}

	public int getParameterCount ()
	{
		return 0;
	}

	public Object getActualParameter (int index)
	{
		return null;
	}

	public Class getFormalParameter (int index)
	{
		return null;
	}
}

