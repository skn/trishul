package trishul.test.taint.array;

class Options
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	// Test if array taint options are set as expected

		int[] array = new int [5];
    	array[0] = taint (1, 0x03);
    	array[1] = taint (1, 0x10);

    	// Test the TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE option
		if (getConfigOption ("TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE"))
		{
	    	checkArrayTaint ("TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE on", array, 0x13);
		}
		else
		{
	    	checkArrayTaint ("TRISHUL_TAINT_ARRAY_ON_MEMBER_STORE off", array, 0);
		}

		taintArray (array, 0x75);

    	// Test the TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD option
		if (getConfigOption ("TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD"))
		{
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD on 0", array[0], 0x75 | 0x03);
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD on 1", array[1], 0x75 | 0x10);
		}
		else
		{
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD off 0", array[0], 0x03);
	    	checkTaint ("TRISHUL_TAINT_MEMBER_ON_ARRAY_LOAD off 1", array[1], 0x10);
		}
	}
}
