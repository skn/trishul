package trishul_p;

import japa.parser.ast.expr.Expression;
import japa.parser.ast.type.Type;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

public class ActionPattern
{
	public static class FormalParameter
	{
		/// null means any type
		private Type	type;

		/// name of local variable created to hold actual value
		private String	name;

		/// null means match any (or no) taint.
		private TaintPattern taint;

		/**
		 *
		 */
		public FormalParameter (Type type, String name, TaintPattern taint)
		{
			this.type	= type;
			this.name	= name;
			this.taint	= taint;
		}

		public Type getType ()
		{
			return type;
		}

		public boolean hasType ()
		{
			return (type != null);
		}

		public String getName ()
		{
			return name;
		}

		public boolean hasName ()
		{
			return (name != null);
		}

		public TaintPattern getTaint ()
		{
			return taint;
		}

		public boolean hasTaint ()
		{
			return (taint != null);
		}
	}

	private boolean							isAbstractAction;
	private boolean 						isFinal;
	private List<FormalParameter>			formals = new LinkedList<FormalParameter> ();
	private boolean							ellipses;
	private TaintPattern					ellipsesTaint;
	private Type							returnType;
	private String 							packageName;
	private String 							className;
	private String 							methodName;
	private TaintPattern 					thisTaint;
	private TaintPattern 					contextTaint;

	public ActionPattern ()
	{
	}

	public void setAbstractAction (boolean isAbstractAction)
	{
		this.isAbstractAction = isAbstractAction;
	}

	public boolean isAbstractAction ()
	{
		return isAbstractAction;
	}

	public String getAbstractActionName ()
	{
		return packageName + "." + className + "." + methodName;
	}

	public void setFinal (boolean isFinal)
	{
		this.isFinal = isFinal;
	}

	public boolean isFinal ()
	{
		return isFinal;
	}

	public void setMethod (String packageName, String className, String methodName, TaintPattern thisTaint)
	{
		this.packageName	= packageName;
		this.className		= className;
		this.methodName		= methodName;
		this.thisTaint		= thisTaint;
	}

	public String getPackageName ()
	{
		return packageName;
	}

	public boolean hasPackageName ()
	{
		return (packageName != null);
	}

	public String getClassName ()
	{
		return className;
	}

	public boolean hasClassName ()
	{
		return (className != null);
	}

	public String getMethodName ()
	{
		return methodName;
	}

	public boolean hasMethodName ()
	{
		return (methodName != null);
	}

	public TaintPattern getThisTaint ()
	{
		return thisTaint;
	}

	public boolean hasThisTaint ()
	{
		return (thisTaint != null);
	}

	public void setContextTaint (TaintPattern contextTaint)
	{
		this.contextTaint = contextTaint;
	}

	public TaintPattern getContextTaint ()
	{
		return contextTaint;
	}

	public boolean hasContextTaint ()
	{
		return (contextTaint != null);
	}

	public void addFormalParameter (FormalParameter formal)
	{
		formals.add (formal);
	}

	public Collection<FormalParameter> formalParameters ()
	{
		return formals;
	}

	public boolean hasFormalParameters ()
	{
		return !formals.isEmpty ();
	}

	public void allowAnyFormalParameter (TaintPattern taint)
	{
		ellipses 		= true;
		ellipsesTaint 	= taint;
	}

	public boolean isAnyParameterAllowed ()
	{
		return ellipses;
	}

	public TaintPattern getAnyParameterTaint ()
	{
		return ellipsesTaint;
	}

	public boolean hasAnyParameterTaint ()
	{
		return (ellipsesTaint != null);
	}

	public void setReturnType (Type type)
	{
		this.returnType = type;
	}

	public Type getReturnType ()
	{
		return returnType;
	}

	public boolean hasReturnType ()
	{
		return (returnType != null);
	}

	public String toString ()
	{
		String s = "<";

		// Modifiers
		if (isFinal) s += "final";
		if (isAbstractAction)
		{
			if (isFinal) s += ", ";
			s += "final";
		}
		if (isFinal || isAbstractAction)
		{
			s += " ";
		}

		// Return type
		if (returnType == null)
		{
			s += "*";
		}
		else
		{
			s += returnType.toString ();
		}

		// Method name
		s += " ";
		if (packageName == null)
			s += "*";
		else
			s += packageName;
		s += ".";
		if (className == null)
			s += "*";
		else
			s += className;
		if (thisTaint != null)
			s += thisTaint.toString ();
		s += ".";
		if (methodName == null)
			s += "*";
		else
			s += methodName;

		// Formal parameters
		s += "(";
		boolean first = true;

		for (FormalParameter fp: formals)
		{
			if (!first) s += ", ";

			if (fp.type == null)
				s += "*";
			else
				s += fp.type.toString ();

			if (fp.name != null)
				s += " " + fp.name;

			if (fp.taint != null)
				s += fp.taint.toString ();

			first = false;
		}

		if (ellipses)
		{
			if (!first) s += ", ";
			s += "..";
			if (ellipsesTaint != null)
				s += ellipsesTaint.toString ();
		}
		s += ")";

		if (contextTaint != null)
			s += contextTaint.toString ();

		s += ">";
		return s;
	}


}

