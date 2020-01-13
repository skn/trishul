
abstract class StringBufferOutput
extends StringOutput
{
	protected StringBuffer sb;

	StringBufferOutput ()
	{
		sb = new StringBuffer ();
	}

	void finish ()
	throws Exception
	{
		init (sb.toString ());
	}
}
