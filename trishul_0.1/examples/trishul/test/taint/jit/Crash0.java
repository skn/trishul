package trishul.test.taint.jit;
import java.math.*;

class Crash0
extends trishul.test.taint.TaintTest
{
	public static void main (String[] args)
	{
		BigInteger.valueOf (0);
	}
}
