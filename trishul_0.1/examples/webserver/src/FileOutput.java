import java.io.*;
import java.util.*;

class FileOutput
extends InputStreamOutput
{
	private File 			file;
	private Server 			server;
	private Authenticator 	authenticator;

	FileOutput (Server server, Authenticator authenticator, File file)
	throws FileNotFoundException
	{
		super (new FileInputStream (file));

		this.file 			= file;
		this.server			= server;
		this.authenticator	= authenticator;
	}

	Authenticator getAuthenticator ()
	{
		return authenticator;
	}

	String getMIMEType ()
	{
		return server.getMIMEType (file);
	}

	long getLength ()
	{
		return file.length ();
	}

	Date getLastModified ()
	{
		return new Date (file.lastModified ());
	}

	void finish ()
	{
	}

}

