package trishul.test.misc;

import org.apache.commons.logging.impl.LogFactoryImpl;

class Log4jTest
{
	public static void main(String[] args)
	{
		//LogFactoryImpl f = new LogFactoryImpl();
		//f.getLogConstructor();
		org.apache.commons.logging.LogFactory fac = org.apache.commons.logging.LogFactory.getFactory();
        fac.getInstance(Log4jTest.class);
	}
}