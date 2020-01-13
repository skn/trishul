package trishul.annotate;

import japa.parser.Location;
import japa.parser.ast.*;
import japa.parser.ast.body.*;
import japa.parser.ast.expr.*;
import japa.parser.ast.stmt.*;
import japa.parser.ast.type.*;
import japa.parser.ast.visitor.ModifierVisitor;

import java.util.LinkedList;

class AnnotateVisitor
extends ModifierVisitor
{
	private static final String 	CLASS_ANNOTATION_BASE 	= "trishul.TrishulAnnotation";
	private static final String 	PACKAGE_ANNOTATION		= "trishul.annotations";
	private static final String 	PREFIX_CTOR				= "ctorHook$";
	private static final String 	PREFIX_METHOD			= "methodHook$";
	private static final String 	PREFIX_NOTAINT_VAR		= "notaintvar$";
	private static final String 	PREFIX_NOTAINT_METHOD	= "notaintmethod$";
	private static final String 	PREFIX_NOTAINT_FALLBACK	= "notaintfallback$";
	private static final String 	NAME_THIS				= "$this";
	private static final String 	TYPE_THIS				= "This";
	private static final String 	NAME_INVOKE_HOOKED		= "$super";

	private MethodDeclaration 				currentMethod;
	private ClassOrInterfaceDeclaration		currentClass;
	private	String							inPackage;

	public Object visit (CompilationUnit n, Object arg)
	throws Exception
    {
    	// Import the original package
   		/*if (n.imports == null)
   			n.imports = new LinkedList<ImportDeclaration> ();
   		n.imports.add (new ImportDeclaration (Location.BUILTIN, n.pakage, false, true));*/

		// Rewrite the package name
    	String packageSuffix = "";
    	if (n.pakage != null)
    	{
    		packageSuffix = "." + n.pakage.toString ();
    		inPackage = n.pakage.toString ();
    	}
   		n.pakage = new NameExpr (Location.BUILTIN, PACKAGE_ANNOTATION + packageSuffix);

    	super.visit (n, arg);

    	return n;
    }

    public Object visit (ClassOrInterfaceDeclaration n, Object arg)
	throws Exception
    {
    	currentClass = n;
    	if (n instanceof TrishulTaintDeclaration)
    	{
	    	n.extend = new ClassOrInterfaceType (Location.BUILTIN, null, CLASS_ANNOTATION_BASE, null);
    	}
   		return super.visit (n, arg);
    }

 	public Object visit (ConstructorDeclaration n, Object arg)
	throws Exception
 	{
 		super.visit (n, new Boolean (false));
		MethodDeclaration meth = new MethodDeclaration (n.getLocation (),
														ModifierSet.FINAL | ModifierSet.PUBLIC | ModifierSet.STATIC,
 														n.annotations, n.typeParameters, new VoidType (Location.BUILTIN),
 														PREFIX_CTOR, n.parameters, 0, n.throws_, n.block);
		rewriteFormals (meth, false);
		return meth;
 	}

 	public Object visit (FieldDeclaration n, Object arg)
	throws Exception
 	{
 		/* Is this a notrishultaint declaration? */
 		if ((n.modifiers & ModifierSet.NOTRISHULTAINT) != 0)
 		{
			for (VariableDeclarator var : n.variables)
			{
				var.id.name = PREFIX_NOTAINT_VAR + var.id.name;
			}
 		}

		return super.visit (n, arg);
 	}

 	public Object visit (MethodDeclaration n, Object arg)
	throws Exception
 	{
 		/* Is this a notrishultaint declaration? */
 		if ((n.modifiers & ModifierSet.NOTRISHULTAINT) != 0)
 		{
 			n.name = PREFIX_NOTAINT_METHOD + n.name;
 			n.modifiers |= ModifierSet.FINAL | ModifierSet.NATIVE;
 			return super.visit (n, arg);
 		}
 		else if ((n.modifiers & ModifierSet.NOTRISHULFALLBACK) != 0)
 		{
 			n.name = PREFIX_NOTAINT_FALLBACK + n.name;
 			n.modifiers |= ModifierSet.FINAL | ModifierSet.NATIVE;
 			return super.visit (n, arg);
 		}

 		currentMethod = n;
 		super.visit (n, new Boolean ((n.modifiers & ModifierSet.STATIC) != 0));

		rewriteFormals (n, (n.modifiers & ModifierSet.STATIC) != 0);
		n.modifiers = ModifierSet.FINAL | ModifierSet.PUBLIC | ModifierSet.STATIC;
		n.name		= PREFIX_METHOD + n.name;
 		return n;
 	}

 	private void rewriteFormals (MethodDeclaration n, boolean isStatic)
 	{
 		if (n.parameters == null)
 			n.parameters = new LinkedList<Parameter> ();

		if (!isStatic)
		{
	 		Parameter paramThis = new Parameter (Location.BUILTIN, 0, null,
	 											 new ClassOrInterfaceType (Location.BUILTIN, null, TYPE_THIS, null), false,
	 											 new VariableDeclaratorId (Location.BUILTIN, NAME_THIS, 0));
			n.parameters.add (0, paramThis);

		}
 	}

    public Object visit (ThisExpr n, Object arg)
	throws Exception
    {
    	super.visit (n, arg);
    	return new NameExpr (n.getLocation (), NAME_THIS);
    }

	/**
	 * Convert a super () to an invocation of the original method.
	 */
    public Object visit (ExplicitConstructorInvocationStmt n, Object arg)
	throws Exception
    {
    	super.visit (n, arg);

		if (n.args == null)
		{
			n.args = new LinkedList<Expression> ();
		}

		// Expects boolean isStatic as arg
		if (!((Boolean) arg).booleanValue ())
		{
			n.args.add (0, new NameExpr (n.getLocation (), NAME_THIS));
		}
		else
		{
			n.args.add (0, new NullLiteralExpr (n.getLocation ()));
		}

    	MethodCallExpr expr = new MethodCallExpr (n.getLocation (),
    											  null, null, NAME_INVOKE_HOOKED, n.args);
    	return new ExpressionStmt (n.getLocation (), expr);
	}

	private String lookupSuperPostfix (Type type)
	{
		if (type.toString ().equals ("int")) 		return "I";
		if (type.toString ().equals ("long")) 		return "J";
		if (type.toString ().equals ("double")) 	return "D";
		if (type.toString ().equals ("float")) 		return "F";
		if (type.toString ().equals ("char")) 		return "C";
		if (type.toString ().equals ("short")) 		return "S";
		if (type.toString ().equals ("byte")) 		return "B";
		if (type.toString ().equals ("boolean")) 	return "Z";
		return null;
	}

	/**
	 * Convert a a=super () to an invocation of the original method.
	 */
    public Object visit (MethodCallExpr n, Object arg)
	throws Exception
    {
    	if (n.name == null && n.scope instanceof SuperExpr)
    	{
    		n.scope = new NameExpr (n.getLocation (), currentClass.name);
    		n.name = NAME_INVOKE_HOOKED;

    		String postfix = lookupSuperPostfix (currentMethod.type);
    		if (postfix != null)
    			n.name += postfix;

			if (n.args == null)
			{
				n.args = new LinkedList<Expression> ();
			}

			// Expects boolean isStatic as arg
			if (!((Boolean) arg).booleanValue ())
			{
				n.args.add (0, new NameExpr (n.getLocation (), NAME_THIS));
			}
			else
			{
				n.args.add (0, new NullLiteralExpr (n.getLocation ()));
	    	}

			if (postfix == null)
			{
				n.typeArgs = new LinkedList<Type> ();
				n.typeArgs.add (currentMethod.type);
			}

		}

    	return super.visit (n, arg);
	}

    public Object visit (ClassOrInterfaceType n, Object arg)
	throws Exception
    {
    	if (n.name.equals (currentClass.name))
    	{
    		if (inPackage != null)
    			n.name = inPackage + "." + n.name;
    	}
    	return super.visit (n, arg);
    }

}
