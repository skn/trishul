import java.io.*;

abstract class StringOutput
extends Output
{
	private String 	s;
	private byte[]	b;

	StringOutput ()
	{
	}

	StringOutput (String s)
	{
		init (s);
	}

	protected void init (String s)
	{
		this.s = s;
		b = s.getBytes ();
	}

	InputStream getStream ()
	throws Exception
	{
		return new ByteArrayInputStream (b);
	}

	long getLength ()
	{
		return b.length;
	}
}
