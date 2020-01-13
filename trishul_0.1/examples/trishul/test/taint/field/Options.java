package trishul.test.taint.field;

class Options
extends trishul.test.taint.TaintTest
{
	private	int		i;

    public static void main(String args[])
    {
    	// Test if object taint options are set as expected
    	Options m = new Options ();

    	m.i = taint (27, 0x03);

    	// Test the TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE option
		if (getConfigOption ("TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE"))
		{
	    	checkObjectTaint ("TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE on", m, 0x03);
		}
		else
		{
	    	checkObjectTaint ("TRISHUL_TAINT_OBJECT_ON_MEMBER_STORE off", m, 0);
		}

		taintObject (m, 0x70);

    	// Test the TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD option
		if (getConfigOption ("TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD"))
		{
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD on", m.i, 0x73);
		}
		else
		{
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_OBJECT_LOAD off", m.i, 0x03);
		}
    }
}
