package trishul.test.trishul_p.compound;

public class Test
{
	public static void main (String[] args)
	{
		System.out.println ("A");
		hook (true);
		System.out.println ("B");
		hook (false);
		System.out.println ("C");
	}

	static void hook (boolean b) {}

}
