package trishul.test.taint.misc;

class misc1
extends trishul.test.taint.TaintTest
{
	static void test (int i)
	throws Exception
	{
    	switch (i)
      	{
      	case 220:                  // hello
        	break;
      	default:
	        throw new Exception();
      	}
    }

    public static void main(String args[])
    {
    	try
    	{
    		test (0);
    	}
    	catch (Exception e)
    	{

    	}

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
