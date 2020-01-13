package trishul_p;

/**
 *
 */
public class CompoundOrder
extends Order
{
	private Order[] orders;

	public CompoundOrder (Order[] orders)
	{
		this.orders = orders;
	}

	public Order[] getOrders ()
	{
		return orders;
	}

	public int getType ()
	{
		return TYPE_COMPOUND;
	}
}
