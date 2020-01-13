import java.io.*;
import java.util.*;

public class SSIFilter
extends Filter
{
	public Output getOutput (Server server, Output input, Request request)
	throws Exception
	{
		return new SSIOutput (server, input, request);
	}

	private class SSIOutput
	extends StringBufferOutput
	{
		private final static String SSI_OPEN 			= "<!--#";
		private final static String SSI_CLOSE 			= "-->";
		private final static String SSI_ECHO 			= "echo";
		private final static String SSI_ECHO_VAR		= "var";
		private final static String SSI_INCLUDE			= "include";
		private final static String SSI_INCLUDE_VIRTUAL = "virtual";
		private final static String SSI_INCLUDE_FILE	= "file";

		private Output 			source;
		private Request 		request;
		private Server 			server;

		SSIOutput (Server server, Output source, Request request)
		{
			this.source  		= source;
			this.request 		= request;
			this.server	 		= server;
		}

		Authenticator getAuthenticator ()
		{
			return source.getAuthenticator ();
		}

		public void finish ()
		throws Exception
		{
			BufferedReader buf = new BufferedReader (new InputStreamReader (source.getStream ()));
			for (;;)
			{
				String line = buf.readLine ();
				if (line == null)
					break;

				int start = 0;
				for (;;)
				{
					int pos = line.indexOf (SSI_OPEN, start);
					if (pos < 0)
						break;
					sb.append (line.substring (start, pos));

					start = pos + SSI_OPEN.length  ();
					int end = line.indexOf (SSI_CLOSE, start);
					if (end < 0)
						break;

					handleCommand (line.substring (start, end));
					start = end + SSI_CLOSE.length ();
				}

				sb.append (line.substring (start));
				sb.append ("\n");
			}

			super.finish ();
		}

		String		buffer;
		int			pos;

		String		key;
		String		val;

		private boolean readToken ()
		{
			while (pos < buffer.length () && Character.isWhitespace (buffer.charAt (pos)))
				pos++;

			// Read key
			int start = pos;
			if (pos <  buffer.length () && Character.isLetter (buffer.charAt (pos)))
				pos++;
			while (pos < buffer.length () && Character.isLetterOrDigit (buffer.charAt (pos)))
				pos++;

			if (pos == start)
				return false;

			key = buffer.substring (start, pos);

			while (pos < buffer.length () && Character.isWhitespace (buffer.charAt (pos)))
				pos++;
			if (pos < buffer.length () && buffer.charAt (pos) == '=')
			{
				pos++;
				while (pos < buffer.length () && Character.isWhitespace (buffer.charAt (pos)))
					pos++;
				if (pos >= buffer.length () || buffer.charAt (pos) != '"')
					return false;
				pos++;

				start = pos;

				while (pos < buffer.length () && buffer.charAt (pos) != '"')
					pos++;
				if (pos >= buffer.length ())
					return false;

				val = buffer.substring (start, pos);
			}
			return true;
		}

		private void handleCommand (String line)
		throws Exception
		{
			Properties 	params = new Properties ();
			buffer 	= line;
			pos		= 0;

			String cmd;

			if (readToken ())
			{
				cmd = key;

				while (readToken ())
				{
					params.setProperty (key, val);
				}

				if (cmd.equals (SSI_ECHO))
				{
					String varname = params.getProperty (SSI_ECHO_VAR);
					if (varname != null)
					{
						sb.append (request.getProperty (varname));
					}
					else
					{
						// TODO: is this an error?
					}
				}
				else if (cmd.equals (SSI_INCLUDE))
				{
					if (params.getProperty (SSI_INCLUDE_FILE) != null)
					{
						handleIncludeFile (params.getProperty (SSI_INCLUDE_FILE), false);
					}
					else if (params.getProperty (SSI_INCLUDE_VIRTUAL) != null)
					{
						handleIncludeFile (params.getProperty (SSI_INCLUDE_VIRTUAL), true);
					}
					else
					{
						// TODO: is this an error?
					}
				}
				else
				{
					// Unknown command.
					// TODO: is this an error?
				}
			}
		}

		private void handleIncludeFile (String path, boolean isVirtual)
		throws Exception
		{
			Request request;
			if (isVirtual)
			{
				request = new Request (path);
			}
			else
			{
				File newPath = new File (new File (this.request.getPath ()).getParent (), path);
				request = new Request (newPath.toString ());
			}

			Output output = server.locateFile (request);
			InputStream in = output.getStream ();
			byte[] buffer = new byte[1024];
			for (;;)
			{
				int count = in.read (buffer);
				if (count < 0)
					break;
				sb.append (new String (buffer, 0, count));
			}
		}

		String getMIMEType ()
		{
			return "text/html";
		}

		Date getLastModified ()
		{
			return source.getLastModified ();
		}

	}

}