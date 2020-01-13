package trishul.test.taint.leak;

class leakB
extends trishul.test.taint.TaintTest
{
    public static void main(String args[])
    {
    	int a = taint (0, 1);
    	int b;

    	switch (a)
    	{
    	case 0:  b=0; break;
    	case 1:  b=1; break;
    	case 2:  b=2; break;
    	case 7:  b=13; break;
    	case 24: b=24; break;
    	default: b=9;
    	}

    	checkTaint ("context leak", b, 0x01);

    	checkTaint ("fallback", getFallbackTaint (), 0);
	}
}
