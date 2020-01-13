package trishul_p;

/**
 *
 */
public abstract class TaintOrder
extends Order
{
	private Taint 	taint;

	public TaintOrder (Taint taint)
	{
		this.taint = taint;
	}

	public Taint getTaint ()
	{
		return taint;
	}
}
