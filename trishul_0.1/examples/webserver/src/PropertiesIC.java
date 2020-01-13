class PropertiesIC
extends java.util.Properties
{
	public Object put (Object key, Object val)
	{
		return super.put (((String) key).toUpperCase (), val);
	}

	public Object get (Object key)
	{
		return super.get (((String) key).toUpperCase ());
	}
}
