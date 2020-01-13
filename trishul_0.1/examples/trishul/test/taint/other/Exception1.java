package trishul.test.taint.other;

import java.io.*;

public class Exception1
extends trishul.test.taint.TaintTest
{
    public static void main(String[] args)
    {
		int i = taint(23,0x01);
		if (i > 20) {
    		try {
     			BufferedReader in = new BufferedReader(new FileReader("infilename"));
     			in.close();
    		} catch (IOException e) {
    			//e.printStackTrace ();
      			checkTaint ("Exception reference", e, 0x01);
      			checkObjectTaint ("Exception", e, 0x01);
    		}
		}

    	//checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
