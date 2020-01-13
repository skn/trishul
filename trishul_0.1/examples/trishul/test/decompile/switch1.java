package trishul.test.decompile;

class switch1
{
	public static void main (String[] args)
	{
		int i;
		
		switch (args.length)
		{
		case 1:		i = 1;	break;
		case 2:		i = 2;	break;
		case 3:		i = 3;	break;
		case 4:		i = 4;	break;
		case 5:		i = 5;	break;
		default:
			if (args.length == 7)
			{
				i = 7;
				break;
			}
			i = 8;
		}
		
	}
	
}
