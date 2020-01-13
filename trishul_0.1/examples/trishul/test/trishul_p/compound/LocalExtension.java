package trishul.test.trishul_p.compound;

public class LocalExtension
{
	public void run ()
	{
		try
		{
			java.io.FileInputStream in = new java.io.FileInputStream ("top-secret.txt");
			byte[] buffer = new byte[1024];
			int read = in.read (buffer, 0, buffer.length);
			System.out.print (new String (buffer, 0, read) + "\n");
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}
}