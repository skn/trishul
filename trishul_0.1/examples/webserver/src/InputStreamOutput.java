import java.io.*;

abstract class InputStreamOutput
extends Output
{
	private InputStream in;

	InputStreamOutput (InputStream in)
	{
		this.in = in;
	}

	InputStream getStream ()
	throws Exception
	{
		return in;
	}

}

