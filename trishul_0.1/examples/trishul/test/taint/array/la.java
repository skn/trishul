package trishul.test.taint.array;

class la
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
		long array[] = new long[10];

		int i = taint (3, 0x04); checkTaint ("Init", i, 0x04);

		// Check if the index is included in the taint
		checkTaint ("Index taint", array[i], 0x04);

		// Check if the array is included in the taint
		//taintArray (array, 0x02);
		//checkTaint ("Array taint", array[i], 0x06);

		// Check if the value is included in the taint
		array[i] = taint ((long) 7, 0x01);
		checkTaint ("Value taint", array[i], 0x05);

		// Check if other values are unaffected
		checkTaint ("Unaffected value", array[1], 0);

		// Check if the value is cleared properly
		array[i] = 8;
		checkTaint ("Value taint 2", array[i], 0x04);

		// Check if the index taint is stored
		checkTaint ("Stored index taint", array[3], 0x04);

		// Check if the value is cleared properly
		array[3] = (long) 8;
		checkTaint ("Cleared taint", array[3], 0);

		// Check if the value is set properly
		array[3] = taint ((long) 9, 0x01);
		checkTaint ("Value taint 3", array[3], 1);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
