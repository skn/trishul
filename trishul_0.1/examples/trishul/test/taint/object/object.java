package trishul.test.taint.object;

class object
extends trishul.test.taint.TaintTest
{
	private	boolean	b;
	private	short	s;
	private	char	c;
	private	int		i;
	private	long	l;
	private	double	d;
	private	float	f;
	private	Object	a;

	int getReturnValue ()
	{
		return taint (255, 0x10);
	}

    public static void main(String args[])
    {
    	object obj = new object ();
    	taintObject (obj, 0x3);

    	checkTaint ("return value", obj.getReturnValue (), 0x13);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
