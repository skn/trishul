package trishul_p;
import java.util.LinkedList;
import java.util.List;

public class CompoundEngine
extends Engine
{
	private List<Engine>	engines = new LinkedList<Engine> ();

	public CompoundEngine ()
	{
	}

	/**
	 * Adds an engine to the end of the list.
	 */
	public void addEngine (Engine newEngine)
	{
		engines.add (newEngine);
	}

	/**
	 *
	 */
	public void addEngine (Engine after, Engine newEngine)
	{
		int index = engines.indexOf (after);
		if (index < 0)
			throw new IllegalArgumentException ("Engine not found");
		engines.add (index + 1, newEngine);
	}

	public void removeEngine (Engine remove)
	{
		// This is done using a copy, to avoid a ConcurrentModificationException
		int oldSize = engines.size ();
		List<Engine>	engines = new LinkedList<Engine> ();
		engines.addAll (this.engines);
		engines.remove (remove);
		if (oldSize == engines.size ())
			throw new IllegalArgumentException ("Engine not found");
		this.engines = engines;
	}

	public Order query (Action a)
	{
		Order 			result 		= null;
		List<Order> 	compound 	= new LinkedList<Order> ();

	outOfLoop:
		for (Engine engine: engines)
		{
			Order partial = engine.doQuery (a);
			if (partial != null)
			{
				result = partial;
				compound.add (partial);

				switch (partial.getType ())
				{
				case Order.TYPE_HALT:
				case Order.TYPE_REPLACE:
				case Order.TYPE_SUPPRESS:
				case Order.TYPE_EXCEPTION:
					break outOfLoop;
				}
			}
		}

		if (compound.size () > 1)
		{
			result = new CompoundOrder (compound.toArray (new Order[0]));
		}

		return result;
	}

}
