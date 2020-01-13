package trishul.test.taint.jit;
import java.math.*;

class Crash2
extends trishul.test.taint.TaintTest
{
	public static void main (String[] args)
	{
		valueOf (0);
	}

	static void valueOf (long val)
	{
	    if (val >= -1000 && val <= 1024)
    	  return;
	}
}
