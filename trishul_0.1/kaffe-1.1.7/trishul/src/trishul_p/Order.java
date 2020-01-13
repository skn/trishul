package trishul_p;

public abstract class Order
{
	public static final int TYPE_OK						= 0;
	public static final int TYPE_HALT					= 1;
	public static final int TYPE_INSERT					= 2;
	public static final int TYPE_REPLACE				= 3;
	public static final int TYPE_SUPPRESS				= 4;
	public static final int TYPE_EXCEPTION				= 5;
	public static final int TYPE_TAINT_PARAMETER		= 6;
	public static final int TYPE_UNTAINT_PARAMETER		= 7;
	public static final int TYPE_TAINT_RETURN_VALUE		= 8;
	public static final int TYPE_UNTAINT_RETURN_VALUE	= 9;
	public static final int TYPE_TAINT_OBJECT			= 10;
	public static final int TYPE_UNTAINT_OBJECT			= 11;
	public static final int TYPE_COMPOUND				= 12;
	public static final int TYPE_UNTAINT_FALLBACK		= 13;
	public static final int TYPE_BREAKPOINT				= 14;
	public static final int TYPE_TAINTDEBUGLEVEL		= 15;

  	public abstract int getType ();

  	protected Order ()
  	{
	}

	// Filled in by kaffe
	protected Action triggerAction;
}

