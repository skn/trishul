package trishul.test;
import trishul.*;

/**
 * To run: ./kaffe-tst -Xtainter trishul.test.HookTest\$TestTainter trishul.test.HookTest
 */
class HookTest
{
	static public void main (String[] args)
	{
		System.gc ();
		staticHookedMethod ("FAIL");
		new HookTest ().hookedMethod ("FAIL");
		System.gc ();
		new CtorTest ("FAIL").check ();
		System.gc ();
		System.out.println ("Done");
	}

	static class CtorTest
	{
		String stored;

		CtorTest (String s)
		{
			stored = s;
		}

		void check ()
		{
			System.out.println (stored);
		}
	}

	static void staticHookedMethod (String s)
	{
		System.out.println (s);
	}

	void hookedMethod (String s)
	{
		System.out.println (s);
	}

	static class TestTainter
	extends Tainter
	{
		TestTainter ()
		throws Exception
		{
			registerHook (HookTest.class, "staticHookedMethod", new Class[] {String.class}, null, null, "hook_staticHookedMethod");
			registerHook (HookTest.class, "hookedMethod", new Class[] {String.class}, null, null, "hook_hookedMethod");
			registerHook (CtorTest.class, null, new Class[] {String.class}, null, null, "hook_CtorTest");
		}

		void hook_staticHookedMethod (String s, java.lang.reflect.Method _hooked)
		throws Exception
		{
			invokeMethod (_hooked, null, new Object[] {"OK1"});
		}

		void hook_hookedMethod (String s, java.lang.reflect.Method _hooked, HookTest _this)
		throws Exception
		{
			invokeMethod (_hooked, _this, new Object[] {"OK2"});
		}

		void hook_CtorTest (String s, java.lang.reflect.Constructor _hooked, CtorTest _this)
		throws Exception
		{
			invokeConstructor (_hooked, _this, new Object[] {"OK3"});
		}
	}
}