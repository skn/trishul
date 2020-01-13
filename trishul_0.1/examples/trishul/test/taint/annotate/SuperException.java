package trishul.test.taint.annotate;

class SuperException
{
    public static void main(String args[])
    {
    	try
    	{
    		ThrowException1 ();
    	}
    	catch (java.io.IOException e1)
    	{
	    	try
	    	{
	    		ThrowException2 ();
	    	}
	    	catch (java.io.IOException e2)
	    	{
	    		if (!e2.getMessage ().equals ("Test3"))
	    		{
			    	System.err.println ("Exception not intercepted, are annotations installed?");
	    		}

				return;
	    	}
    	}
    	System.err.println ("Exception not thrown or caught");
    }

    static void ThrowException1 ()
    throws java.io.IOException
    {
    	throw new java.io.IOException ("Test1");
    }

    static void ThrowException2 ()
    throws java.io.IOException
    {
    	throw new java.io.IOException ("Test2");
    }


}
