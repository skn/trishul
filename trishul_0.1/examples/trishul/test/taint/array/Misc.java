package trishul.test.taint.array;

class Misc
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
		int array[] = new int[taint (3, 0x04)];
		checkArrayTaint ("Init", array, 0x04);
		checkTaint 		("Length", array.length, 0x04);

		String sarray[] = new String[taint (3, 0x07)];
		checkArrayTaint ("String init", sarray, 0x07);

		// Test if reference taint is included in length
		array = (int[]) taint (new int[3], 0x03);
		checkTaint 		("Length reference", array.length, 0x03);

		array = new int[3];
		checkTaint 		("Length cleared", array.length, 0);

		array = new int[3]; taintArray (array, 0x30);
		checkTaint 		("Length array", array.length, 0x30);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
