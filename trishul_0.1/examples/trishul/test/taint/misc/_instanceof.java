package trishul.test.taint.misc;

class _instanceof
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	Object o = taint (new String ("s"), 0xFF);
    	checkTaint ("init", o, 0xFF);
    	checkTaint ("instanceof true", o instanceof String, 0xFF);
    	checkTaint ("instanceof false", o instanceof Integer, 0xFF);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
