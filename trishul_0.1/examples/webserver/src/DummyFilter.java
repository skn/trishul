/**
 * For debug purposes. Passes the output through the filter.
 */
class DummyFilter
extends Filter
{
	Output getOutput (Server server, Output input, Request request)
	throws Exception
	{
		return input;
	}
}

