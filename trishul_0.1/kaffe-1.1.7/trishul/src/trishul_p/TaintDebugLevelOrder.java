package trishul_p;

/**
 * Sets the current taint debug level. Purely for debugging purposes.
 */
public class TaintDebugLevelOrder
extends Order
{
	private int debugLevel;

 	public TaintDebugLevelOrder (int debugLevel)
  	{
  		this.debugLevel = debugLevel;
  	}

	public int getType ()
	{
		return TYPE_TAINTDEBUGLEVEL;
	}

	public int getDebugLevel ()
	{
		return debugLevel;
	}
}
