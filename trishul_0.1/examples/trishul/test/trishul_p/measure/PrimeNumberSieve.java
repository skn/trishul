package trishul.test.trishul_p.measure;

import java.io.*;

class PrimeNumberSieve
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
		//for(int j=1;j<4100;j=j*2) {
		//for(int j=1;j<20;j=j*2) {
			int n_loops = 16*1024;
			int n_primes = 0;

			int j;

			long start_time = System.currentTimeMillis();

			//for(j=0; j < n_loops; j++)
			//j = taint(0,0x03);
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

			long end_time = System.currentTimeMillis();

			System.out.println(n_primes + " primes found in " + n_loops + " integers in " + (end_time - start_time)  + "ms ");
			//System.out.println("Time taken = " + (end_time - start_time));
		//}
	}
}

