abstract class Filter
{
	abstract Output getOutput (Server server, Output input, Request request)
	throws Exception;
}

