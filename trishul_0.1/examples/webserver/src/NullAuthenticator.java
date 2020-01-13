class NullAuthenticator
extends Authenticator
{
	Response handleRequest (Request request)
	throws Exception
	{
		return null;
	}

	boolean isAuthenticated ()
	{
		return true;
	}

	String getUser ()
	{
		return null;
	}
}
