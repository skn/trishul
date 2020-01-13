import java.io.BufferedReader;
import java.util.Properties;
import java.util.StringTokenizer;

class Request
{
	private static final String METHODS[] = {"GET", "POST"};
	static final int METHOD_GET 	= 0;
	static final int METHOD_POST 	= 1;
	private static final String VERSIONS[] = {"HTTP/1.0", "HTTP/1.1"};

	private int				method		= -1;
	private int				version		= -1;
	private String			path;
	private Properties		params		= new Properties ();
	private Properties		properties	= new Properties ();

	Request ()
	{
	}

	Request (String path)
	{
		try
		{
			init (METHODS[0], path, VERSIONS[0]);
		}
		catch (Response r) {}
	}

	private void init (String methodString, String path, String versionString)
	throws Response
	{
		int i;
		this.properties = properties;

		for (i = 0; i < METHODS.length && method < 0; i++)
			if (METHODS[i].equals (methodString))
				method = i;

		for (i = 0; i < VERSIONS.length && version < 0; i++)
			if (VERSIONS[i].equals (versionString))
				version = i;

		this.path = path;

		if (method  < 0 ||
		 	version < 0 )
		{
			version = 0;
			method  = 0;
			throw Response.newBadRequest (this);
		}
	}

	void read (BufferedReader in)
	throws Exception
	{
		String 			line 	= in.readLine ();
		StringTokenizer tokens 	= new StringTokenizer (line, " \t");

		init (tokens.nextToken (), tokens.nextToken (), tokens.nextToken ());
		if (tokens.hasMoreTokens ())
			throw Response.newBadRequest (this);

		for (;;)
		{
			String s = in.readLine ();
			if (s.length () == 0)
				break;

			int pos = s.indexOf (": ");
			if (pos < 0)
				throw Response.newBadRequest (this);
			String key = s.substring (0, pos).trim ();
			String val = s.substring (pos + 1).trim ();
			if (key.length () == 0 ||
				val.length () == 0)
				throw Response.newBadRequest (this);

			params.setProperty (key.toUpperCase (), val);
			properties.setProperty ("HTTP_" + key.replace ('-', '_'), val);
		}

		// TODO: POST handling
	}

	String getPath ()
	{
		return path;
	}

	String getVersionString ()
	{
		return VERSIONS[version];
	}

	String getParam (String name)
	{
		return params.getProperty (name.toUpperCase ());
	}

	String getProperty (String name)
	{
		return properties.getProperty (name.toUpperCase ());
	}

	void setProperty (String name, String value)
	{
		properties.setProperty (name.toUpperCase (), value);
	}
}
