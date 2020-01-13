import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;
import java.net.InetAddress;
import java.net.Socket;

class Connection
{
	private Server			server;
	private Socket			client;
	private BufferedReader	in;
	private PrintStream		out;

    public Connection (Server server, Socket client)
    throws IOException
	{
		this.server	= server;
	    this.client = client;
		in  = new BufferedReader (new InputStreamReader (client.getInputStream ()));
		out	= new PrintStream (client.getOutputStream ());
    }

    public InetAddress getRemoteHost ()
    {
    	return client.getInetAddress ();
    }

	public void run()
	{
		boolean keepAlive = false;
		do
		{
			try
			{
				Response response;
				try
				{
					response = server.handleRequest (this, readRequest ());
				}
				catch (Response errorResponse)
				{
					errorResponse.printStackTrace ();
					response = errorResponse;
				}

				response.write (out);
			}
			catch (Exception e)
			{
				e.printStackTrace ();
				keepAlive = false;
			}
		} while (keepAlive);

		try
		{
			in.close  ();
			out.close ();
			client.close ();
		}
		catch (IOException e)
		{
		}
	}

	private Request readRequest ()
	throws Response
	{
		Request request = new Request ();

		try
		{
			request.read (in);
			return request;
		}
		catch (Response r)
		{
			throw r;
		}
		catch (Exception e)
		{
			e.printStackTrace ();
			throw Response.newBadRequest (request);
		}
	}
}