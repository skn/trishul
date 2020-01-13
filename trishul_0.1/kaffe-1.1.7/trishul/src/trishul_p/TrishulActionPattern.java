package trishul_p;

/**
 * A simple structure that is used to pass information into trishul.
 * If modifier == 4 (abstract), only returnType is used, which contains the abstract action's class.
 */
public class TrishulActionPattern
{
	public static class Parameter
	{
		// null means match any type
		Class			type;
		TaintPattern	taint;

		public Parameter (Class type, TaintPattern taint)
		{
			this.type  = type;
			this.taint = taint;
		}
	}

	int 			modifier;
	Class 			returnType;
	String 			packageName;
	String 			className;
	TaintPattern	thisTaint;
	String 			methodName;
	Parameter[] 	parameters;
	boolean			anyParameters;
	TaintPattern	anyParametersTaint;
	TaintPattern	contextTaint;
	private	int		handle;

	public TrishulActionPattern (int modifier, Class returnType,
								 String packageName, String className, String methodName, TaintPattern thisTaint,
								 Parameter[] parameters, boolean anyParameters,
								 TaintPattern anyParametersTaint, TaintPattern contextTaint)
	{
		this.modifier			= modifier;
		this.returnType			= returnType;
		this.packageName		= packageName;
		this.className			= className;
		this.methodName			= methodName;
		this.thisTaint			= thisTaint;
		this.parameters			= parameters;
		this.anyParameters		= anyParameters;
		this.anyParametersTaint	= anyParametersTaint;
		this.contextTaint		= contextTaint;
	}
}

