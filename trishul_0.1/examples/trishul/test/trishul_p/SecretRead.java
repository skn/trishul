package trishul.test.trishul_p;

import java.io.*;

class SecretRead
{

	static void main (String args[])
	{
		try
		{
			FileInputStream in     = new FileInputStream (args[0]);
			DataInputStream reader = new DataInputStream (in);
			//System.out.println (reader.readLine ());
			String line = reader.readLine();
            int index;
            for (index = 0; index < line.length(); index++) {
            	char token= line.charAt(index);
                System.out.println(token);
            }
		}
		catch (Exception e)
		{
			e.printStackTrace ();
		}
	}
}
