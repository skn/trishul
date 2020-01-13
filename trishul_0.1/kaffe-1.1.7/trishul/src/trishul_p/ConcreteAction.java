package trishul_p;

import java.util.StringTokenizer;

public class ConcreteAction
extends Action
{
	private String 		packageName;
	private String 		methodName;
	private String 		className;
	private String 		signature;
	private Object 		thisPointer;
	private int			methodHandle;
	private int			actualParameterHandle;
	private Object[] 	actualParameters;
	private Class[] 	formalParameters;

	/**
	 * Private constructor for kaffe only.
	 */
  	private ConcreteAction (Object thisPointer,
  							String packageName, String className, String methodName, String signature,
  							int methodHandle, int actualParameterHandle, Class[] formalParameters)
	{
		this.thisPointer			= thisPointer;
		this.packageName 			= packageName;
		this.methodName 			= methodName;
		this.className 				= className;
		this.signature 				= signature;
		this.methodHandle			= methodHandle;
		this.actualParameterHandle	= actualParameterHandle;
		this.formalParameters		= formalParameters;
	}

	/**
	 * Public constructor to create actions that can be inserted.
	 * TODO: should this be a separate class, or perhaps specified using a language construct? Anyway, more of
	 *       the work done here can be done at compile time.
	 */
	public ConcreteAction (Object thisPointer, String fullMethName, Object[] actualParameters)
	{
	    try
	    {
			this.actualParameters = actualParameters;
			this.thisPointer = thisPointer;
			int i=fullMethName.indexOf("(");
			String ps = fullMethName.substring(i+1,fullMethName.length()-1);
			fullMethName = fullMethName.substring(0,i);
			i=fullMethName.lastIndexOf(".");
			methodName = fullMethName.substring(i+1);
			fullMethName = fullMethName.substring(0,i);
			i=fullMethName.lastIndexOf(".");
			if (i > 0)
			{
				packageName = fullMethName.substring(0,i);
				className = fullMethName.substring(i+1);
			}
			StringTokenizer t = new StringTokenizer(ps,",");
			StringBuffer sigBuffer = new StringBuffer ();
			sigBuffer.append ('(');
			formalParameters = new Class[t.countTokens()];
			i=0;
			while (t.hasMoreTokens())
			{
				formalParameters[i] = Types.stringToClass(t.nextToken().trim());
				sigBuffer.append (Types.classToString (formalParameters[i]));
				i++;
			}

			sigBuffer.append (')');
			signature = sigBuffer.toString ();
		}
		catch(Exception e)
		{
			System.out.println("<Polymer> Error creating action object:");
			e.printStackTrace();
			System.exit(1);
		}
	}

	public String getPackageName()
	{
		return packageName;
	}

	public String getClassName()
	{
		return className;
	}

	public String getMethodName()
	{
		return methodName;
	}

	public String getSignature()
	{
		return signature;
	}

  	public Object getThisPointer ()
  	{
  		return thisPointer;
  	}

	public int getParameterCount ()
	{
		// This uses formalParameters to avoid having to load the param array
		return formalParameters.length;
	}

	public Object getActualParameter (int index)
	{
		ensureParametersLoaded ();
		return actualParameters[index];
	}

	public Object[] getActualParameters ()
	{
		ensureParametersLoaded ();
		return actualParameters;
	}

	private final void ensureParametersLoaded ()
	{
		if (actualParameters == null)
		{
			actualParameters = loadParameters (methodHandle, actualParameterHandle);
		}
	}

	private final native static Object[] loadParameters (int methodHandle, int actualParameterHandle);

	public Class getFormalParameter (int index)
	{
		ensureParametersLoaded ();
		return formalParameters[index];
	}

  	public String toString ()
  	{
    	if (formalParameters != null)
    	{
      		String s = packageName + "." + className + "." + methodName + "(";
      		for (int i = 0; i < formalParameters.length; i++)
      			s += formalParameters[i].toString () + ",";
      		s += ")";
      		return s;
    	}
    	else
    	{
    		return packageName + "." + className + "." + methodName + " " + signature;
    	}
  	}

  	public String toSimpleString()
  	{
    	return packageName + "." + className + "." + methodName;
  	}
}

