package trishul.test.taint.jit;
import java.io.*;

class Crash1
extends trishul.test.taint.TaintTest
{
	public static void main (String[] args)
	{
		new File (".", "javac-asm");
	}
}
