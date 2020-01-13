package trishul.test.trishul_p.measure;

import java.text.DateFormat;
import java.util.*;

class PrintDate
{
	static void main (String args[])
	{
		DateFormat dateFormatter = DateFormat.getInstance ();
		System.out.println (dateFormatter.format (new Date (1900)));
	}
}
