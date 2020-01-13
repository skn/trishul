abstract class Authenticator
{
	/**
	 * Validates any authentication information passed along with the request.
	 * If a response is returned, this is returned to the client. If null is returned, the request is executed.
	 */
	abstract Response handleRequest (Request request)
	throws Exception;

	abstract boolean isAuthenticated ();

	abstract String getUser ();
}
