package trishul.test.taint.annotate;

trishultaint SuperException
{
    static void ThrowException1 ()
    throws java.io.IOException
    {
    	SuperException.<java.io.IOException>$superX (null);
    }

    static void ThrowException2 ()
    throws java.io.IOException
    {
    	try
    	{
	    	SuperException.<java.io.IOException>$superX (null);
    	}
    	catch (java.io.IOException e)
    	{
    		throw new java.io.IOException ("Test3");
    	}
    }
}
