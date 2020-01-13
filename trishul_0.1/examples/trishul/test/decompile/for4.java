package trishul.test.decompile;

class for4
{
	public static void main (String[] args)
	{
		int i;
		
		for (i = 0; i < args.length; i++)
		{
			if (args[i] == "break")
			{
				System.out.println (args[i]);
				break;
			}
			else
			{
				System.out.println (args[i]);
			}
		}
		
	}
	
}
