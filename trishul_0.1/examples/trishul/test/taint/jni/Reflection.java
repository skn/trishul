package trishul.test.taint.jni;
import java.lang.reflect.*;

class Reflection
extends trishul.test.taint.TaintTest
{
	public static void main (String[] args)
	{
		// This fails correctly, because of the non-taken branch taint in Method.invoke
	/*
		try
		{
			Reflection r = new Reflection ();
			Object param;
			Object[] p;
			Method m;

			param = null;
			m = r.getClass ().getDeclaredMethod ("test", new Class[] {Object.class});
			m.invoke (r, new Object[] {param});

			param = taint (null, 0x1);
			checkTaint ("param taint init", param, 0x1);
			m = Reflection.class.getDeclaredMethod ("testParamTaint", new Class[] {Object.class});
			p = new Object[] {param};
			checkTaint ("param taint before", p[0], 0x1);
			m.invoke (r, p);

			((Reflection) taint (r, 0x05)).testThisTaint (null);

			param = null;
			checkTaint ("param taint 2a", param, 0);
			m = Reflection.class.getDeclaredMethod ("testThisTaint2", new Class[] {Object.class});
			p = new Object[] {param};
			checkTaint ("param taint 2", param, 0);
			checkTaint ("this taint before", p[0], 0);
			m.invoke (taint (r, 0x03), p);

			checkTaint ("r taint", r, 0);
			m = Reflection.class.getDeclaredMethod ("testRetValTaint", null);
			checkTaint ("m taint", m, 0);
			checkTaint ("r.getClass ()", r.getClass (), 0);
			checkTaint ("retval taint", ((Integer) m.invoke (r, new Object[] {})).intValue (), 0x10);

			m = r.getClass ().getDeclaredMethod ("testRetValTaint", null);
			checkTaint ("retval object taint", m.invoke (r, new Object[] {}), 0x10);
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
*/

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}

	public int test (Object obj)
	{
		return 1;
	}

	public int testParamTaint (Object obj)
	{
		checkTaint ("param taint", obj, 0x1);
		return 1;
	}

	public int testThisTaint (Object obj)
	{
		checkTaint ("this taint", this, 0x5);
		return 1;
	}

	public int testThisTaint2 (Object obj)
	{
		checkTaint ("this taint", this, 0x3);
		return 1;
	}
	public int testRetValTaint ()
	{
		return taint (1, 0x10);
	}

}
