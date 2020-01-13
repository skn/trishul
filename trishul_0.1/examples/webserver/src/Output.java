import java.io.*;
import java.util.*;

abstract class Output
{
	abstract InputStream getStream ()
	throws Exception;

	abstract Authenticator getAuthenticator ();

	abstract String getMIMEType ();
	abstract long getLength ();
	abstract Date getLastModified ();
	abstract void finish ()
	throws Exception;
}
