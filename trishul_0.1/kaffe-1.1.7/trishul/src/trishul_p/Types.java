package trishul_p;

import java.util.Hashtable;

public class Types
{
	private static class Primitive
	{
		String	name;
		String	objectName;
		String	unwrapMethod;
		int		size;
		Class	cls;
		String	sig;

		Primitive (String name, String objectName, String unwrapMethod, int	size, Class cls, String sig)
		{
			this.name			= name;
			this.objectName		= objectName;
			this.unwrapMethod	= unwrapMethod;
			this.size			= size;
			this.cls			= cls;
			this.sig			= sig;
		}

	}

	private static final Primitive[] PRIMITIVE =
	{
		new Primitive ("int", 		"java.lang.Integer", 	"intValue", 	1, int.class, 		"I"),
		new Primitive ("double", 	"java.lang.Double", 	"doubleValue", 	2, double.class, 	"D"),
		new Primitive ("float", 	"java.lang.Float", 		"floatValue", 	1, float.class, 	"F"),
		new Primitive ("short", 	"java.lang.Short", 		"shortValue", 	1, short.class,		"S"),
		new Primitive ("long", 		"java.lang.Long", 		"longValue", 	2, long.class, 		"J"),
		new Primitive ("byte", 		"java.lang.Byte", 		"byteValue", 	1, byte.class, 		"B"),
		new Primitive ("void", 		"java.lang.Void", 		null, 			0, void.class, 		"V"),
		new Primitive ("char", 		"java.lang.Character", 	"charValue", 	1, char.class, 		"C"),
		new Primitive ("boolean", 	"java.lang.Boolean", 	"booleanValue", 1, boolean.class, 	"Z"),
	};

	private static Hashtable PRIMITIVES_BY_NAME  = new Hashtable ();
	private static Hashtable PRIMITIVES_BY_CLASS = new Hashtable ();

	static
	{
		for (int i = 0; i < PRIMITIVE.length; i++)
		{
			PRIMITIVES_BY_NAME.put 	(PRIMITIVE[i].name, PRIMITIVE[i]);
			PRIMITIVES_BY_CLASS.put (PRIMITIVE[i].cls, PRIMITIVE[i]);
		}
	}


  	public static boolean isPrimitiveType (String type)
  	{
  		return (PRIMITIVES_BY_NAME.get (type) != null);
  	}


	/**
	 * Returns the classname of the object to wrap the primitive type.
	 * For example, when typ is int, this returns "java.lang.Integer".
	 */
	public static String primitive2Object (String type)
	{
		Primitive primitive = (Primitive) PRIMITIVES_BY_NAME.get (type);
		if (primitive == null)
			return type;
		return primitive.objectName;
  	}


  	/** Returns name of method used to unwrap the primitive type.
     * For example, when typ is int, this returns "intValue".
     */
  	public static String getUnwrapMethodName (String type)
  	{
		Primitive primitive = (Primitive) PRIMITIVES_BY_NAME.get (type);
		if (primitive == null)
			return type;
		return primitive.unwrapMethod;
  	}


  	/**
     * Returns number of words required to store the given type.
     */
  	public static int getSizeOf (String type)
  	{
		Primitive primitive = (Primitive) PRIMITIVES_BY_NAME.get (type);
		if (primitive == null)
			return 1;
		return primitive.size;
  	}


	/**
	* Translates a string representation of a type into the
	* corresponding class.  Assumes that the string is a base/object
	* type name followed by zero or more "[]".
	*
	* @todo Verify format assumption is correct.
	*/
	public static Class stringToClass (String s)
	{
		int count = 0;
		for (int i=0; i<s.length(); i++)
		{
		  	if (s.charAt(i) == '[') count++;
		}

		String base = "";
		if (count > 0)
			base = s.substring (0, s.indexOf ("["));
		else base = s;

		Class c = null;
		Primitive prim = (Primitive) PRIMITIVES_BY_NAME.get (base);
		if (prim != null)
		{
			c = prim.cls;
		}
		else
		{
			try
			{
		  		c = Class.forName (base);
			}
			catch (ClassNotFoundException e)
			{
				System.out.println("<Polymer> error creating class object:");
		  		e.printStackTrace();
		  		System.exit(1);
		  	}
		}

		if (count > 0)
		{
			int[] ar = new int[count];
		  	for (int j=0; j<count; j++)
		  		ar[j] = 1;
		  	Object o = java.lang.reflect.Array.newInstance(c,ar);
		  	c = o.getClass();
		}
		return c;
	}

	public static String classToString (Class c)
	{
		if (c.isArray ()) return "[" + classToString (c.getComponentType ());
		else if (c.isPrimitive ()) return ((Primitive) PRIMITIVES_BY_CLASS.get (c)).sig;
		else return "L" + c.getName ().replace ('.', '/') + ";";
	}
}


