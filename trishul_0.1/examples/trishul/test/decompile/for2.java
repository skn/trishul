package trishul.test.decompile;

class for2
{
	public static void main (String[] args)
	{
		int i;
		
		for (i = 0; i < args.length; i++)
		{
			if (i == 1)
			{
				break;
			}
			else
			{
				System.out.println (args[i]);
			}
		}
		
	}
	
}
