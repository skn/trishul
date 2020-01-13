import java.io.*;
import java.net.*;
import java.text.*;
import java.util.*;

/**
 *
 * Security issues:
 *  - .. is allowed in paths.
 *
 */
public class Server
{
	private static final String DIR_WWW_ROOT 		= "www";
	private static final String FILE_CONFIG  		= "www.ini";
	private static final String DEFAULT_PORT 		= "8080";

	private static final String PROP_HOME_ROOT			= "HOME_ROOT";
	private static final String PROP_WWW_ROOT			= "WWW_ROOT";
	private static final String PROP_LISTEN_PORT		= "LISTEN_PORT";
	private static final String PROP_DEFAULT_URL		= "DEFAULT_URL";
	private static final String PROP_MIME_TYPES			= "MIME_TYPES";
	private static final String PROP_ERROR_URL			= "ERROR_URL";
	private static final String PROP_SERVER_NAME		= "SERVER_NAME";
	private static final String PROP_SERVER_SOFTWARE	= "SERVER_SOFTWARE";

	private static final String MIME_DEFAULT		= "application/octet-stream";
	private static final String MIME_IMAGE_DEFAULT	= "/images/text.gif";

	private static final String FILE_AUTHINFO		= "AUTHENTICATION";


	private Properties		properties;
	private PropertiesIC	mimeTypes;
	private Hashtable		filters;
	private ServerSocket 	serverSocket;
	private Output 			errorOutput;

    private Server (String rootPath)
    {
        try
        {
	    	properties = new Properties ();
	    	properties.setProperty (PROP_HOME_ROOT, 		rootPath);
	    	properties.setProperty (PROP_WWW_ROOT,  		DIR_WWW_ROOT);
	    	properties.setProperty (PROP_LISTEN_PORT,  		DEFAULT_PORT);
	    	properties.setProperty (PROP_MIME_TYPES,		"mime.types");
	    	properties.setProperty (PROP_ERROR_URL,			"error.html");
	    	properties.setProperty (PROP_SERVER_SOFTWARE,	"");
	    	properties.setProperty (PROP_SERVER_NAME,		InetAddress.getLocalHost ().getHostName ());
	    	properties.load (new FileInputStream (makeConfigPath (rootPath, FILE_CONFIG)));

	    	mimeTypes = new PropertiesIC ();
	    	mimeTypes.load (new FileInputStream (makeConfigPath (rootPath, properties.getProperty (PROP_MIME_TYPES))));

	    	filters = new Hashtable ();
	    	filters.put ("text/shtml", new SSIFilter ());

	    	errorOutput = locateFile (new Request (properties.getProperty (PROP_ERROR_URL)));

            serverSocket = new ServerSocket (getPort ());
        }
        catch (Exception e)
        {
        	System.err.println (e.getMessage ());
        	System.exit (1);
        }
	}

    private void run ()
    {
        try
        {
        	System.out.println ("Started. Root directory: " + getWWWRoot ());

		    while(true)
		    {
			    Socket 		clientSocket 	= serverSocket.accept();
            	Connection 	connection		= new Connection (this, clientSocket);
            	connection.run ();
            }
	    }
	    catch (Exception e)
	    {
	    	e.printStackTrace ();
	    }
    }

    Response handleRequest (Connection connection, Request request)
    {
		try
		{
	   		Output output = locateFile (request);
	   		Response authResponse = output.getAuthenticator ().handleRequest (request);
	   		if (authResponse != null)
	   			return authResponse;
	   		if (!output.getAuthenticator ().isAuthenticated ())
	   			return Response.newNotAllowed (request);

			setRequestProperties (connection, request, output);
			output.finish ();
   			Response r = Response.newOK (request);
   			r.setData (output);
   			return r;
   		}
   		catch (java.io.FileNotFoundException e)
   		{
   			return makeErrorResponse (Response.newNotFound (request));
   		}
   		catch (Exception e)
   		{
   			e.printStackTrace ();
   			return makeErrorResponse (Response.newInternalServerError (request));
   		}
    }

    private void setRequestProperties (Connection connection, Request request, Output output)
    {

    	Date now = new Date ();
    	DateFormat format = DateFormat.getDateTimeInstance ();

    	request.setProperty ("REMOTE_HOST", 	connection.getRemoteHost ().getHostName ());
    	request.setProperty ("REMOTE_ADDR", 	connection.getRemoteHost ().getHostAddress ());
    	request.setProperty ("SERVER_NAME",		properties.getProperty (PROP_SERVER_NAME));
    	request.setProperty ("SERVER_SOFTWARE",	properties.getProperty (PROP_SERVER_SOFTWARE));
    	request.setProperty ("DOCUMENT_URI",	request.getPath ());
    	request.setProperty ("DATE_LOCAL", 		format.format (now));
    	request.setProperty ("LAST_MODIFIED", 	format.format (output.getLastModified ()));
    	// TODO: DOCUMENT_NAME, REQUEST_STRING
    }

    Response makeErrorResponse (Response r)
    {
    	r.setData (errorOutput);
    	return r;
    }

    Output locateFile (Request request)
    throws Exception
    {
    	File file = new File (translatePath (request.getPath ()));
    	if (file.isDirectory ())
    	{
    		StringTokenizer tokens = new StringTokenizer (properties.getProperty (PROP_DEFAULT_URL, ","));
    		while (tokens.hasMoreTokens ())
    		{
    			File index = new File (file, tokens.nextToken ());
    			if (index.canRead ())
    			{
    				return getFileHandler (index, request);
    			}
    		}

    		return new DirectoryIndexOutput (this, authenticatorFromPath (file), file, request);
    	}
    	else
    	{
    		return getFileHandler (file, request);
    	}
    }

    Output getFileHandler (File file, Request request)
    throws Exception
    {
   		String mime   = getMIMEType (file);
    	Filter filter = (Filter) filters.get (mime);
		Output output = new FileOutput (this, authenticatorFromPath (file), file);
    	if (filter != null)
    		return filter.getOutput (this, output, request);
    	return output;
    }

    private Authenticator authenticatorFromPath (File path)
    throws Exception
    {
    	if (!path.isDirectory ())
    	{
    		path = new File (path.getParent ());
    	}

    	File authInfo = new File (path, FILE_AUTHINFO);
	   	if (authInfo.exists ())
    	{
    		Properties users = new Properties ();
    		users.load (new FileInputStream (authInfo));
    		return new BasicAuthenticator (path.toString (), users);
    	}

    	return new NullAuthenticator ();
    }

	private String translatePath (String path)
	{
		String s = getWWWRoot ();
		if (!s.endsWith (File.separator))
			s += File.separator;
		s += path;
		// TODO: scan for ..
		return s;
	}

	private static String makeConfigPath (String s1, String s2)
	{
		File f2 = new File (s2);
		if (f2.isAbsolute ())
			return s2;

		String s = s1;
		if (!s1.endsWith (File.separator))
			s += File.separator;
		s += s2;
		return s;
	}

    int getPort ()
    {
    	return Integer.parseInt (properties.getProperty (PROP_LISTEN_PORT));
    }

    String getWWWRoot ()
    {
    	return makeConfigPath (properties.getProperty (PROP_HOME_ROOT), properties.getProperty (PROP_WWW_ROOT));
    }

	String getMIMEType (File file)
	{
		String extension;
		int pos = file.getName ().lastIndexOf (".");
		if (pos >= 0)
			extension = file.getName ().substring (pos + 1).toUpperCase ();
		else
			extension = "";
		String mime = mimeTypes.getProperty (extension);
		if (mime == null)
			return MIME_DEFAULT;

		return mime.substring (0, mime.indexOf (","));
	}

	String getMIMEImage (File file)
	{
		String extension;
		if (file.isFile ())
		{
			int pos = file.getName ().lastIndexOf (".");
			if (pos >= 0)
				extension = file.getName ().substring (pos + 1).toUpperCase ();
			else
				extension = "";
		}
		else
		{
			extension = "";
		}

		String mime = mimeTypes.getProperty (extension);
		if (mime == null)
			return MIME_IMAGE_DEFAULT;
		return mime.substring (mime.indexOf (",") + 1);
	}

    static public void main(String args[])
    {
        try
	    {
	    	if (args.length < 1)
	    	{
	    		System.err.println ("Usage: Server <rootdir>\n");
	    		System.exit (1);
	    	}

			Server server = new Server (args[0]);
			server.run ();
	    }
	    catch (Exception e)
	    {
	    	e.printStackTrace ();
	    }
    }
}

