package trishul.test.trishul_p;

class println2
{
	static void main (String[] args)
	{
		try
		{
			System.out.println (666);
			System.out.println ("NOT ALLOWED");
			System.out.println (getString ());
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}

	static String getString ()
	{
		return "Orignal string";
	}
}