package trishul_p;

/**
 * Causes a trap into the debugger. Purely for debugging purposes.
 */
public class BreakpointOrder
extends Order
{
 	public BreakpointOrder ()
  	{
  	}

	public int getType ()
	{
		return TYPE_BREAKPOINT;
	}
}
