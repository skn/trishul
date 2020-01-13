import java.io.*;
import java.util.*;
import java.text.DateFormat;

class Response
extends Exception
{
	private int					code;
	private String				message;
	private Properties			properties;
	private Output		 		data;
	private Request				request;
	private static DateFormat	dateFormatter = DateFormat.getInstance ();

	private Response (Request request, int code, String message)
	{
		this.request	= request;
		this.code 		= code;
		this.message 	= message;
		properties		= new Properties ();
	}

	void setData (Output data)
	{
		this.data 			= data;
		properties.setProperty ("Content-Type", data.getMIMEType ());

		long length = data.getLength ();
		if (length >= 0)
			properties.setProperty ("Content-Length", Long.toString (length));

		Date lastModified = data.getLastModified ();
		properties.setProperty ("Last-Modified", dateFormatter.format (lastModified));
	}

	void setProperty (String name, String val)
	{
		properties.setProperty (name, val);
	}

	static Response newOK (Request request) 					{return new Response (request, 200, "OK");}
	static Response newBadRequest (Request request)				{return new Response (request, 400, "Bad request");}
	static Response newAuthorizationRequired (Request request)	{return new Response (request, 401, "Authorization required");}
	static Response newNotAllowed (Request request)				{return new Response (request, 403, "Not allowed");}
	static Response newNotFound (Request request)				{return new Response (request, 404, "Not found");}
	static Response newVersionNotSupported (Request request)	{return new Response (request, 505, "HTTP Version not supported");}
	static Response newInternalServerError (Request request)	{return new Response (request, 500, "Internal server error");}

	void write (PrintStream out)
	throws Exception
	{
		out.println (request.getVersionString () + " " + Integer.toString (code) + " "  + message);
		for (Enumeration e = properties.propertyNames (); e.hasMoreElements (); )
		{
			String name = (String) e.nextElement ();
			out.print (name);
			out.print (": ");
			out.println (properties.getProperty (name));
		}

		out.println ();

		if (data != null)
		{
			InputStream in = data.getStream ();
			byte[] buffer = new byte[1024];
			for (;;)
			{
				int count = in.read (buffer);
				if (count < 0)
					break;
				out.write (buffer, 0, count);
			}
		}
	}
}
