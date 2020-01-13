package trishul;
import java.lang.reflect.Constructor;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

abstract public class Tainter
extends Engine
{
	protected Tainter ()
	{
	}

	/**
	 * Registers a hook. The hook is invoked in stead of the normal method or constructor. It is responsible
	 * for invoking the original, through the passed Method object or invokeConstructor in case of a constructor-hook.
	 *
	 * TODO: currently if the Tainter is implemented in an inner class which is not static, kaffe starts without
	 *       using it or displaying a warning
	 *
	 * @param	cls			The class whose method should be hooked
	 * @param	methodName	The method to hook, or null to hook a constructor
	 * @param	sig			Array of classes that indicates the method's parameter signature
	 * @param	onObject	If not null, only when the method is invoked on the specified object is the hook called.
	 * @param	hookClass	The class containing the hook, or null if it is the subclass of Tainter.
	 * @param	hookName	The name of the hook. The method must have the following parameters:
	 *						- The parameters specified by the original method
	 *						- A java.lang.reflect.Method or Constructor, which receives the hooked method.
	 *						- An object of the type of original this-pointer (as passed in cls), unless the hooked
	 *						  method is static
	 */
	protected void registerHook (Class cls, String methodName, Class[] sig,
								 Object onObject, Class hookClass, String hookName)
	throws InvalidHookException, NoSuchMethodException
	{
		if (methodName == null)
		{
			// This is a ctor hook

			if (onObject != null)
			{
				throw new InvalidHookException ("Constructor hooks are not allowed to specify object filters");
			}

			// Find the ctor and hook and and register them
			Constructor ctor	= cls.getDeclaredConstructor (sig);
			Method hook			= findHook (hookClass, hookName, sig, cls, true);
			registerHook (ctor, hook);
		}
		else
		{
			// Find the method and hook and and register them
			Method method 	= cls.getDeclaredMethod (methodName, sig);
			Class  thisType	= Modifier.isStatic (method.getModifiers ()) ? void.class : method.getDeclaringClass ();
			Method hook		= findHook (hookClass, hookName, sig, thisType, false);
			registerHook (method, onObject, hook);
		}
	}

	private Method findHook (Class hookClass, String hookName, Class[] sig,
							 Class thisPtr, boolean isCtor)
	throws NoSuchMethodException
	{
		// Construct the new signature that includes the additional this pointer
		int needSize = sig.length + 1;
		if (thisPtr != void.class) needSize++;

		Class[] newSig = new Class[needSize];
		System.arraycopy (sig, 0, newSig, 0, sig.length);

		int index = sig.length;

		// Add the hooked method
		newSig[index] = isCtor ? Constructor.class : Method.class;
		index++;

		// Add the this pointer
		if (thisPtr != void.class)
		{
			newSig[index] = thisPtr;
			index++;
		}

		if (hookClass == null)
		{
			hookClass = getClass ();
		}

		try
		{
			return hookClass.getDeclaredMethod (hookName, newSig);
		}
		catch (NoSuchMethodException e)
		{
			// Throw a more detailed exception
			StringBuffer sigStr = new StringBuffer ();
			for (int i = 0; i < newSig.length; i++)
			{
				if (i != 0)
				{
					sigStr.append (", ");
				}
				sigStr.append (newSig[i].getName ());
			}
			throw new NoSuchMethodException (hookClass.getName () + "." + hookName + " (" + sigStr + ")");
		}
	}

	protected void registerHook (Constructor ctor, Method hook)
	throws InvalidHookException
	{
		registerHookShared (ctor.getParameterTypes (), void.class,
							ctor.getDeclaringClass (), null, hook, Constructor.class);

		registerHookInt (ctor, null, hook);
	}

	protected void registerHook (Method method, Object onObject, Method hook)
	throws InvalidHookException
	{
		registerHookShared (method.getParameterTypes (), method.getReturnType (),
							Modifier.isStatic (method.getModifiers ()) ?
							void.class : method.getDeclaringClass (),
							onObject, hook, Method.class);

		registerHookInt (method, onObject, hook);

	}

	private void registerHookShared (Class[] paramTypes, Class returnType,
									 Class thisType, Object onObject,
									 Method hook, Class hookClass)
	throws InvalidHookException
	{
		/* Check the hook's return type */
		if (!hook.getReturnType ().equals (returnType))
		{
			throw new InvalidHookException ("Hook method " + hook.getName () +
											" should have return type " + returnType);
		}

		/* Check the param types of the hook match the original parameter types */
		Class[] hookParamTypes = hook.getParameterTypes ();

		int i;
		for (i = 0; i < paramTypes.length; i++)
		{
			if (i >= hookParamTypes.length)
			{
				throw new InvalidHookException ("Hook method " + hook.getName () +
											    " misses parameter " + paramTypes[i]);
			}

			if (!hookParamTypes[i].equals (paramTypes[i]))
			{
				throw new InvalidHookException ("Hook method " + hook.getName () + " has wrong type for parameter " +
						(i + 1) + ", type is " + hookParamTypes[i] + ", should be " + paramTypes[i]);
			}
		}

		/* Check if the next parameter is the hooked method */
		if (i >= hookParamTypes.length)
		{
			throw new InvalidHookException ("Hook method " + hook.getName () +
					" misses parameter for hooked Method object");
		}
		if (!hookParamTypes[i].equals (hookClass))
		{
			throw new InvalidHookException ("Hook method " + hook.getName () + " has wrong type for hooked Method " +
					"object parameter " + (i + 1) + ", type is " + hookParamTypes[i]);
		}
		i++;

		/* Check if the next parameter is the this-pointer, if required */
		if (!thisType.equals (void.class))
		{
			if (i >= hookParamTypes.length)
			{
				throw new InvalidHookException ("Hook method " + hook.getName () +
						" misses parameter for this pointer of type " + thisType);
			}
			if (!hookParamTypes[i].equals (thisType))
			{
				throw new InvalidHookException ("Hook method " + hook.getName () + " has wrong type for this pointer " +
						"parameter " + (i + 1) + ", type is " + hookParamTypes[i] + ", should be " + thisType);
			}
			if (onObject != null && !onObject.getClass ().equals (thisType))
			{
				throw new InvalidHookException ("Hook method " + hook.getName () +
						" uses wrong type for object filter");
			}
			i++;
		}
		else if (onObject != null)
		{
			// Objects can only be checked when there's a this-pointer
			throw new InvalidHookException ("Hook method " + hook.getName () +
					" cannot use object filter, as there is no this pointer");
		}

		/* Check if no more parameters are specified */
		if (i != hookParamTypes.length)
		{
			throw new InvalidHookException ("Hook method " + hook.getName () +
					" should have only " + i + " parameters");
		}
	}

	private native void registerHookInt (Member member, Object onObject, Method hook);

	protected native int 		taint (int value, int taint);
	protected native long 		taint (long value, int taint);
	protected native double 	taint (double value, int taint);
	protected native float 		taint (float value, int taint);
	protected native Object 	taint (Object value, int taint);
	protected native byte 		taint (byte value, int taint);
	protected native short 		taint (short value, int taint);
	protected native boolean 	taint (boolean value, int taint);
	protected native char 		taint (char value, int taint);
	protected native void 		taintObject (Object ref, int taint);
	protected native void 		taintArray 	(Object ref, int taint);

	protected native int		getTaint (int value);
	protected native int		getTaint (long value);
	protected native int		getTaint (double value);
	protected native int		getTaint (float value);
	protected native int		getTaint (Object value);
	protected native int		getTaint (byte value);
	protected native int		getTaint (short value);
	protected native int		getTaint (boolean value);
	protected native int		getTaint (char value);
	protected native int		getObjectTaint (Object ref);
	protected native int		getArrayTaint  (Object ref);

	/**
	 * Allows a constructor to be invoked explicitly, much like Method.invoke.
	 */
	protected native void invokeConstructor (Constructor _hooked, Object _this, Object[] param);
	protected native Object invokeMethod (Method _hooked, Object _this, Object[] param);
}

