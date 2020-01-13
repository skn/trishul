package trishul.test.taint.leak;

class leakC
extends trishul.test.taint.TaintTest
{
	/* Doesn't really test anything, just a weird loop from String.equals that caused problems */
    public static void main(String args[])
    {
    	int i = 0, n = 1, j = 0;

		for (; i < n; i++, j++)
		{
			if (i != j)
			{
				return;
			}
		}
		return;
	}
}
