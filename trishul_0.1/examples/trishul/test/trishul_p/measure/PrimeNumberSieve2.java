package trishul.test.trishul_p.measure;

import java.io.*;

class PrimeNumberSieve2
extends trishul.test.taint.TaintTest
{
	static boolean isPrime(long i)
	{
		for(int test = 2; test < i; test++)
		{
			//checkTaint("mod", i%test,0x02);
			if(i%test == 0)
			{
				return false;
			}
		}
		return true;
	}

	public static void main(String[] args) throws IOException
	{
		doMain ();

		long start_time = System.currentTimeMillis();
		doMain ();
		long end_time = System.currentTimeMillis();
		System.out.println((end_time - start_time)  + "ms ");
	}

	static void doMain ()
	{
			int n_loops = 16*1024;
			int n_primes = 0;

			int j;

			j=0;
			for(; j < n_loops; j++)
			{
				//int i = j;
				//int i; if (j % 2 == 0) i = taint (j, 1); else i = j;
				int i = j;//taint(j,0x02);

				if(isPrime(i))
				{
					//System.out.println("Found prime "+ i);
					n_primes++;
				}
			}

	}
}

