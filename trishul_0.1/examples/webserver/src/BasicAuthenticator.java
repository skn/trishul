import java.util.*;

class BasicAuthenticator
extends Authenticator
{
	private static final String PROP_AUTH 			= "Authorization";
	private static final String PROP_AUTH_REQUEST 	= "WWW-Authenticate";
	private String 		realm;
	private Properties 	users;

	BasicAuthenticator (String realm, Properties users)
	{
		this.realm = realm;
		this.users = users;
	}

	Response handleRequest (Request request)
	throws Exception
	{
		if (request.getParam (PROP_AUTH) != null)
		{
			StringTokenizer tokens = new StringTokenizer (request.getParam (PROP_AUTH));
			tokens.nextToken (); // Skip basic

			if (tokens.hasMoreTokens () && validate (tokens.nextToken ()))
				return null;
		}

		Response response = Response.newAuthorizationRequired (request);
		response.setProperty (PROP_AUTH_REQUEST, "Basic Realm=" + realm);
		return response;
	}

	private boolean validate (String info)
	{
		String decoded = new String (Base64.decode (info));
		int pos = decoded.indexOf (':');
		if (pos < 0)
			return false;

		String user = decoded.substring (0, pos);
		String pass = decoded.substring (pos + 1);
		return pass.equals (users.getProperty (user));
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
