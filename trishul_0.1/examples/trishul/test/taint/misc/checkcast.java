package trishul.test.taint.misc;

class checkcast
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	Object o = taint (new String ("s"), 0xFF);
    	String s = (String) o;
    	checkTaint ("init", o, 0xFF);
    	checkTaint ("checkcast", s, 0xFF);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
