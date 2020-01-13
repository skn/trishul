package trishul_p.compiler;

import japa.parser.Location;
import japa.parser.ast.*;
import japa.parser.ast.body.*;
import japa.parser.ast.expr.*;
import japa.parser.ast.stmt.*;
import japa.parser.ast.type.*;
import japa.parser.ast.visitor.ModifierVisitor;
import java.lang.reflect.Modifier;
import java.util.*;
import trishul_p.*;

/**
 * Converts TrishulP constructs (aswitch) to Java constructs.
 */
class TrishulPVisitor
extends ModifierVisitor
{
	private static final String VAR_PATTERNS			= "__TRISHUL_PATTERNS";
	private static final String METHOD_MATCHES 			= "__TRISHUL_MATCHES";
	private static final String CLASS_ENGINE			= "Engine";
	private static final String CLASS_ABSTRACT_ACTION	= "AbstractAction";
	private static final String CLASS_PARSED_PATTERN	= "trishul_p.TrishulActionPattern";
	private static final String TAINT_AUTO				= "trishul_p.Taint.TAINT_AUTO";
	private static final String PREFIX_FORMAL_PARAM		= "param";

	private static final ClassOrInterfaceType	TYPE_ACTION_DONE	= new ClassOrInterfaceType (Location.BUILTIN, null,
																				"trishul_p.DoneAction", null);
	private static final ClassOrInterfaceType	TYPE_TAINT			= new ClassOrInterfaceType (Location.BUILTIN, null,
																				"trishul_p.Taint", null);
	private static final ClassOrInterfaceType	TYPE_TAINT_PATTERN	= new ClassOrInterfaceType (Location.BUILTIN, null,
																				"trishul_p.TaintPattern", null);
	private static final ClassOrInterfaceType	TYPE_FORMAL			= new ClassOrInterfaceType (Location.BUILTIN, null,
															CLASS_PARSED_PATTERN + ".Parameter", null);

	private static final ClassOrInterfaceType	JAVA_LANG_OBJECT =
		new ClassOrInterfaceType (Location.BUILTIN, null, "Object", null);

	private Expression			PATTERNS_ARRAY = new NameExpr (Location.BUILTIN, VAR_PATTERNS);

	private List<ActionPattern>	patterns;
	/// Counter used to ensure names are unique
	private int 				aswitchCount;
	private boolean				isEngine;
	private boolean				isAbstractAction;
	private int					abstractCounter;
	private HashSet<String>		engineTaintNames = new HashSet<String> ();

    public Object visit (CompilationUnit n, Object arg)
    throws Exception
    {
    	for (EngineTaintDeclaration taint : n.engineTaints)
    	{
    		engineTaintNames.add (taint.name);
    	}

    	return super.visit (n, arg);
	}

    public Object visit (ClassOrInterfaceDeclaration n, Object arg)
    throws Exception
    {
    	// Save old context, for inner class
		List<ActionPattern>	patterns			= this.patterns;
		int 				aswitchCount		= this.aswitchCount;
		boolean				isEngine			= this.isEngine;
		boolean				isAbstractAction	= this.isAbstractAction;

    	// Setup new context
    	this.patterns 			= new LinkedList<ActionPattern> ();
		this.aswitchCount		= 0;
		this.isEngine			= isEngine (n);
		this.isAbstractAction	= isAbstractAction (n);

    	Object ret = super.visit (n, arg);

		if (!this.patterns.isEmpty ())
		{
			createPatternsArray (n, this.patterns);
		}

		if (this.isAbstractAction)
		{
			createAbstractAction (n);
		}

    	// Restore old context
    	this.patterns 			= patterns;
		this.aswitchCount		= aswitchCount;
		this.isEngine			= isEngine;
		this.isAbstractAction	= isAbstractAction;

    	return ret;
    }

	private void createAbstractAction (ClassOrInterfaceDeclaration n)
	{
		Location l = n.getLocation ();

		// Collect the names and types of formal parameters
		List<VariableDeclarator> paramNames = new LinkedList<VariableDeclarator> ();
		List<FieldDeclaration>   paramTypes = new LinkedList<FieldDeclaration> ();
		int maxParamIndex = 0;
		for (BodyDeclaration bd : n.members)
		{
			if (bd instanceof FieldDeclaration)
			{
				FieldDeclaration fd = (FieldDeclaration) bd;
				for (VariableDeclarator var : fd.variables)
				{
					// TODO: check id.arrayCount, var.init
					if (var.id.name.startsWith (PREFIX_FORMAL_PARAM))
					{
						try
						{
							int index = Integer.parseInt (var.id.name.substring (PREFIX_FORMAL_PARAM.length ()));

							if (index > maxParamIndex)
								maxParamIndex = index;
							paramNames.add (var);
							paramTypes.add (fd);
						}
						catch (NumberFormatException e)
						{
							error (var, "Invalid formal parameter declaration");
							return;
						}
					}
				}
			}
		}

		if (maxParamIndex != paramNames.size ())
		{
			error (n, "Not all formal parameters have been declared");
			return;
		}


		// Create public abstract Object getThisPointer ();
		n.members.add (new FieldDeclaration (l, ModifierSet.PRIVATE, null, JAVA_LANG_OBJECT,
						new VariableDeclarator (l, new VariableDeclaratorId (l, "_this", 0), null)));
		n.members.add (new MethodDeclaration (l, ModifierSet.PUBLIC, null, null,
						JAVA_LANG_OBJECT, "getThisPointer",
						null, 0, null,
						new BlockStmt (l, new ReturnStmt (l, new NameExpr (l, "_this")))));

		// Create public abstract int	   getParameterCount ();
		n.members.add (new MethodDeclaration (l, ModifierSet.PUBLIC, null, null,
						new PrimitiveType (l, PrimitiveType.Primitive.Int), "getParameterCount",
						null, 0, null,
						new BlockStmt (l, new ReturnStmt (l, new IntegerLiteralExpr (l, paramNames.size ())))));

		// Create public abstract Object getActualParameter (int index);
		// Create public abstract Class  getFormalParameter (int index);
		List<Parameter> params = new LinkedList<Parameter> ();
		params.add (new Parameter (l, 0, null, new PrimitiveType (l, PrimitiveType.Primitive.Int), false,
											new VariableDeclaratorId (l, "index", 0)));
		Expression selector = new NameExpr (l, "index");

		List<SwitchEntryStmt> casesActual = new LinkedList<SwitchEntryStmt> ();
		List<SwitchEntryStmt> casesFormal = new LinkedList<SwitchEntryStmt> ();

		Iterator<VariableDeclarator> iterParamNames = paramNames.iterator ();
		Iterator<FieldDeclaration> iterParamTypes = paramTypes.iterator ();
		for (int i = 0; i < paramNames.size (); i++)
		{
			Type formalType = iterParamTypes.next ().type;
			Expression castActual = createCast (new NameExpr (l, iterParamNames.next ().id.name),
												formalType, JAVA_LANG_OBJECT);
			Statement statActual = new ReturnStmt (l, castActual);
			Statement statFormal = new ReturnStmt (l, typeToExpr (formalType));
			casesActual.add (new SwitchEntryStmt (l, new IntegerLiteralExpr (l, i), statActual));
			casesFormal.add (new SwitchEntryStmt (l, new IntegerLiteralExpr (l, i), statFormal));
		}
		Statement returnNull = new ReturnStmt (l, new NullLiteralExpr (l));
		casesActual.add (new SwitchEntryStmt (l, null, returnNull));
		casesFormal.add (new SwitchEntryStmt (l, null, returnNull));

		n.members.add (new MethodDeclaration (l, ModifierSet.PUBLIC, null, null,
						JAVA_LANG_OBJECT, "getActualParameter",
						params, 0, null, new BlockStmt (l, new SwitchStmt (l, selector, casesActual))));
		n.members.add (new MethodDeclaration (l, ModifierSet.PUBLIC, null, null,
						new ClassOrInterfaceType (l, null, "Class", null), "getFormalParameter",
						params, 0, null, new BlockStmt (l, new SwitchStmt (l, selector, casesFormal))));

	}

    private void createPatternsArray (ClassOrInterfaceDeclaration n, List<ActionPattern> patterns)
    {
    	List<Expression> parsed = new LinkedList<Expression> ();
    	ClassOrInterfaceType type = new ClassOrInterfaceType (n.getLocation (), null, CLASS_PARSED_PATTERN, null);

    	for (ActionPattern pat: patterns)
    	{
    		List<Expression> args = new LinkedList<Expression> ();

			// Modifiers
    		int mod = (pat.isAbstractAction () ? Modifier.ABSTRACT : 0) |
    				  (pat.isFinal () ? Modifier.FINAL : 0);
    		args.add (new IntegerLiteralExpr (n.getLocation (), mod));

			if (pat.isAbstractAction ())
			{
				// The abstract action class is encoded in the return type field
	    		args.add (typeToExpr (new ClassOrInterfaceType (n.getLocation (), null,
	    														pat.getAbstractActionName (), null)));

	    		// Method id
	    		args.add (new NullLiteralExpr (Location.BUILTIN));
	    		args.add (new NullLiteralExpr (Location.BUILTIN));
	    		args.add (new NullLiteralExpr (Location.BUILTIN));
			}
			else
			{
				// Return type
	    		args.add (typeToExpr (pat.getReturnType ()));

	    		// Method id
	    		args.add (nameToExpr (pat.hasPackageName () ? pat.getPackageName ().replace ('.', '/') : null));
	    		args.add (nameToExpr (pat.getClassName 		()));
	    		args.add (nameToExpr (pat.getMethodName 	()));
			}

    		// This taint
    		args.add (taintToExpr (pat.getThisTaint ()));

    		// Parameters
    		args.add (formalsToExpr (pat));

    		// Any parameters
    		args.add (new BooleanLiteralExpr (n.getLocation (), pat.isAnyParameterAllowed ()));
    		args.add (taintToExpr (pat.getAnyParameterTaint ()));

    		// Context taint
    		args.add (taintToExpr (pat.getContextTaint ()));

			// Constructor wrapper
			Expression newExpr = new ObjectCreationExpr (n.getLocation (), null, type, null, args, null);
			newExpr.setComment ("// " + pat, true);
			newExpr.setComment ("", false);
    		parsed.add (newExpr);
    	}

		// The array
    	Expression init = new ArrayInitializerExpr (n.getLocation (), parsed);
    	VariableDeclaratorId id 	= new VariableDeclaratorId 	(n.getLocation (), VAR_PATTERNS, 1);
    	VariableDeclarator 	 var 	= new VariableDeclarator 	(n.getLocation (), id, init);
    	n.members.add (new FieldDeclaration (n.getLocation (),
    										 ModifierSet.STATIC | ModifierSet.PRIVATE | ModifierSet.FINAL,
    						  				 null, type, var));
    }

    private Expression formalsToExpr (ActionPattern pat)
    {
    	if (!pat.hasFormalParameters ())
    	{
    		return new NullLiteralExpr (Location.BUILTIN);
    	}
    	else
    	{
    		List<Expression> values = new LinkedList<Expression> ();

    		for (ActionPattern.FormalParameter fp: pat.formalParameters ())
    		{
    			List<Expression> args = new LinkedList<Expression> ();
		   		args.add (typeToExpr  (fp.getType  ()));
    			args.add (taintToExpr (fp.getTaint ()));
    			values.add (new ObjectCreationExpr (Location.BUILTIN, null, TYPE_FORMAL, null, args, null));
    		}

    		return new ArrayCreationExpr (Location.BUILTIN, TYPE_FORMAL, null, 1,
    									new ArrayInitializerExpr (Location.BUILTIN, values));
    	}
    }

    private Expression typeToExpr (Type type)
    {
    	if (type == null)
    	{
    		return new NullLiteralExpr (Location.BUILTIN);
    	}
    	else
    	{
    		return new ClassExpr (Location.BUILTIN, type);
    	}
    }

    private Expression nameToExpr (String name)
    {
    	if (name == null)
    	{
    		return new NullLiteralExpr (Location.BUILTIN);
    	}
    	else
    	{
    		return new StringLiteralExpr (Location.BUILTIN, name);
    	}
	}

	private Expression taintToExpr (TaintPattern taintPattern)
	{
    	if (taintPattern == null)
    	{
    		return new NullLiteralExpr (Location.BUILTIN);
    	}
    	else
    	{
    		// Create the taint object
    		Taint taint = taintPattern.getTaint ();
    		List<Expression> args = new LinkedList<Expression> ();
    		args.add (new IntegerLiteralExpr (Location.BUILTIN, taint.getType  ()));

    		if (taint instanceof NamedTaint)
    		{
				args.add (taintNamesToExpression (Location.BUILTIN, ((NamedTaint) taint).taintNames ()));
    		}
    		else
    		{
    			args.add (new IntegerLiteralExpr (Location.BUILTIN, taint.getTaint ()));
    		}

			Expression taintExpr = new ObjectCreationExpr (Location.BUILTIN, null, TYPE_TAINT, null, args, null);

			// Create the taint pattern object
			args = new LinkedList<Expression> ();
			args.add (taintExpr);
			args.add (new BooleanLiteralExpr (Location.BUILTIN, taintPattern.matchAll ()));
			args.add (nameToExpr (taintPattern.getSaveName ()));
			return new ObjectCreationExpr (Location.BUILTIN, null, TYPE_TAINT_PATTERN, null, args, null);
    	}
	}

    private boolean isEngine (ClassOrInterfaceDeclaration n)
    {
    	if (n.extend != null)
    	{
    		if (n.extend.name.equals (CLASS_ENGINE))
    		{
    			return true;
    		}
    	}
    	return false;
    }

    private boolean isAbstractAction (ClassOrInterfaceDeclaration n)
    {
    	if (n.extend != null)
    	{
    		if (n.extend.name.equals (CLASS_ABSTRACT_ACTION))
    		{
    			return true;
    		}
    	}
    	return false;
    }

	/**
	 * Convert an action switch to Java.
	 */
    public Object visit (PolymerActionSwitchStmt n, Object arg)
    throws Exception
    {
		if (!isEngine && !isAbstractAction)
		{
			error (n, "aswitch only allowed in trishul_p classes");
		}

    	/**
    	 * aswitch (action)
    	 * {
    	 * case expr: break;
    	 * default: break;
    	 * }
    	 *
    	 * is converted to
    	 * do
    	 * {
    	 *   // For each case
	     *   	Object matchResult;
	     * 	  	if (matchResult = label.matches (action))
	     * 		{
	     * 			// case statements
	     * 		}
	     * } while (false)
	     *
	     * To do/while loop is to allow break statements to work as expected
	     *
	     * Falling through cases is not supported, so a check is made to ensure each case ends with a return or
	     * break statement.
	     */

	    List<Statement> statements = new LinkedList<Statement> ();

		for (PolymerActionSwitchEntryStmt entry : n.entries)
		{
			Statement caseStat = (Statement) entry.accept (this, n);
			statements.add (caseStat);
		}

		// The do/while wrapper
		BlockStmt block = new BlockStmt (n.getLocation (), statements);
		DoStmt doWhile = new DoStmt (n.getLocation (), block, new BooleanLiteralExpr (n.getLocation (), false));
		doWhile.setComment ("// aswitch (" + n.selector.toString () + ")", true);
		aswitchCount++;
		return doWhile;
    }

	private boolean isNotFallThrough (Statement s)
	{
		if (s instanceof BreakStmt || s instanceof ReturnStmt)
			return true;
		if (s instanceof IfStmt)
		{
			IfStmt ifStmt = (IfStmt) s;
			return isNotFallThrough (ifStmt.thenStmt) && isNotFallThrough (ifStmt.elseStmt);
		}
		if (s instanceof SwitchStmt)
		{
			boolean haveDefault = false;
			SwitchStmt switchStmt = (SwitchStmt) s;
			for (SwitchEntryStmt child: switchStmt.entries)
			{
				if (!isNotFallThrough (child.stmts.get (child.stmts.size () - 1)))
					return false;
				if (child.label == null)
					haveDefault = true;
			}
			return haveDefault;
		}

		return false;
	}

    public Object visit (PolymerActionSwitchEntryStmt n, Object arg)
    throws Exception
    {
    	Statement last = n.stmts.get (n.stmts.size () - 1);
    	if (isNotFallThrough (last))
    	{
	    	BlockStmt b = handleCase (n, arg);
	    	return b.accept (this, arg);
    	}

    	error (n, "Case fall through not supported");
    	return null;
    }

    private BlockStmt handleCase (PolymerActionSwitchEntryStmt n, Object arg)
    {
		String 					comment;
		PolymerActionSwitchStmt parent = (PolymerActionSwitchStmt) arg;

		Expression expr;
		comment = "// case (" + n.label.toString () + ")";
		ActionPattern pattern = n.label.pattern;
		LinkedList<Statement> insert = new LinkedList<Statement> ();
		LinkedList<Statement> code   = new LinkedList<Statement> ();
		if (pattern instanceof ActionPatternDone)
		{
			expr = new InstanceOfExpr (n.getLocation (), parent.selector, TYPE_ACTION_DONE);
		}
		else
		{
			int index = patterns.size ();
			patterns.add (pattern);

			// The object used to retrieve actual parameters from
			Expression actualThisPointer = parent.selector;

			NameExpr abstractName = null;
			if (pattern.isAbstractAction ())
			{
				if (pattern.getPackageName () == null ||
					pattern.getClassName   () == null ||
					pattern.getMethodName  () == null )
				{
					error (n, "Full class name must be specified for abstract actions");
				}

				abstractName = new NameExpr (n.getLocation (), "__abstractAction" + abstractCounter++);
				// Insert creation statement
				ClassOrInterfaceType typeAA = new ClassOrInterfaceType (n.getLocation (), null,
																		pattern.getAbstractActionName (), null);
			    VariableDeclaratorId nameAA = new VariableDeclaratorId (n.getLocation (), abstractName.name, 0);
			    ObjectCreationExpr   initAA = new ObjectCreationExpr (n.getLocation (), null, typeAA, null,
			    														null, null);
    			VariableDeclarator varDeclAA = new VariableDeclarator (n.getLocation (), nameAA, initAA);
				code.add (new ExpressionStmt (n.getLocation (),
												new VariableDeclarationExpr (n.getLocation (), 0, null,
																			 typeAA, varDeclAA)));
				actualThisPointer = abstractName;
			}

			// Create the pattern test
			List<Expression> args = new LinkedList<Expression> ();
			args.add (new ArrayAccessExpr (n.getLocation (), PATTERNS_ARRAY,
											new IntegerLiteralExpr (n.getLocation (), index)));
			if (pattern.isAbstractAction ())
			{
				args.add (abstractName);
			}

			args.add (parent.selector);
			expr = new MethodCallExpr (n.getLocation (),
										null, null, METHOD_MATCHES, args);

			// Save actual parameters
			int formalIndex = 0;
			for (ActionPattern.FormalParameter formal: pattern.formalParameters ())
			{
				if (formal.hasName ())
				{
					// Get the value from the action interface
					List<Expression> getArgs = new LinkedList<Expression> ();
					getArgs.add (new IntegerLiteralExpr (n.getLocation (), formalIndex));
					Expression assign = new MethodCallExpr (n.getLocation (), actualThisPointer, null,
															"getActualParameter", getArgs);

					// Cast it to the correct type
					Expression cast = createCast (assign, JAVA_LANG_OBJECT, formal.getType ());

					// Save it into a local variable
				    VariableDeclaratorId name = new VariableDeclaratorId (Location.BUILTIN, formal.getName (), 0);
    				VariableDeclarator var = new VariableDeclarator (Location.BUILTIN, name, cast);
					insert.add (new ExpressionStmt (n.getLocation (),
										new VariableDeclarationExpr (n.getLocation (), 0,
																	null, formal.getType (), var)));
				}
				formalIndex++;
			}
		}

		insert.addAll (n.stmts);
		n.stmts = insert;

		// The if-statement
		IfStmt ifStat = new IfStmt (n.getLocation (), expr, new BlockStmt (Location.BUILTIN, n.stmts), null);
		code.add (ifStat);

		BlockStmt newWrapper = new BlockStmt (Location.BUILTIN, code);
		newWrapper.setComment (comment, true);
		return newWrapper;
    }

	private Expression createCast (Expression expr, Type source, Type target)
	{
		if ((target instanceof PrimitiveType) ^ (source instanceof PrimitiveType))
		{
			// (Un)box primitive
			String accessor;
			String box;
			PrimitiveType prim = (PrimitiveType) ((target instanceof PrimitiveType) ? target : source);
			switch (prim.type)
			{
			case Boolean:
				box 		= "Boolean";
				accessor 	= "booleanValue";
				break;
			case Char:
				box 		= "Character";
				accessor 	= "charValue";
				break;
			case Byte:
				box 		= "Byte";
				accessor 	= "byteValue";
				break;
			case Short:
				box 		= "Short";
				accessor 	= "shortValue";
				break;
			case Int:
				box 		= "Integer";
				accessor 	= "intValue";
				break;
			case Long:
				box 		= "Long";
				accessor 	= "longValue";
				break;
			case Float:
				box 		= "Float";
				accessor 	= "floatValue";
				break;
			case Double:
				box 		= "Double";
				accessor 	= "doubleValue";
				break;
			default:
				return null;
			}

			ClassOrInterfaceType boxType = new ClassOrInterfaceType (expr.getLocation (), null, box, null);
			if (target instanceof PrimitiveType)
			{
				// Unbox
				List<Expression> args = new LinkedList<Expression> ();
				CastExpr cast = new CastExpr (expr.getLocation (), boxType, expr);
				return new MethodCallExpr (expr.getLocation (), cast, null, accessor, args);
			}
			else
			{
				// Box
				List<Expression> args = new LinkedList<Expression> ();
				args.add (expr);
				return new ObjectCreationExpr (expr.getLocation (), null, boxType, null, args, null);
			}
		}
		else
		{
			// Plain cast
			return new CastExpr (expr.getLocation (), target, expr);
		}
	}

	/**
	 * Translate taint declaration to list of integer-constant declarations.
	 */
    public Object visit (EngineTaintDeclaration n, Object arg)
    throws Exception
    {
    	List<VariableDeclarator> variables = new LinkedList<VariableDeclarator> ();

		String previousName = n.extend == null ? null : n.extend.name;
    	for (EngineTaintConstantDeclaration entry: n.entries)
    	{
			VariableDeclaratorId id = new VariableDeclaratorId (n.getLocation (), entry.name, 0);

			// Initialize to 1, or (previous << 1).
			Expression init;
			if (previousName == null)
			{
				init = new IntegerLiteralExpr (n.getLocation (), 1);
			}
			else
			{
				init = new BinaryExpr (n.getLocation (),
					new NameExpr (n.getLocation (), previousName),
					new IntegerLiteralExpr (n.getLocation (), 1),
					BinaryExpr.Operator.lShift
				);
			}

			VariableDeclarator var = new VariableDeclarator (n.getLocation (), id, init);
			variables.add (var);

			previousName = entry.name;
    	}

    	// Add the taint name as an int containing the highest value, to allow extending
		VariableDeclaratorId id = new VariableDeclaratorId (n.getLocation (), n.name, 0);
		VariableDeclarator var = new VariableDeclarator (n.getLocation (), id,
														 new NameExpr (n.getLocation (), previousName));
		variables.add (var);

    	Type type = new PrimitiveType (n.getLocation (), PrimitiveType.Primitive.Int);
    	FieldDeclaration field = new FieldDeclaration (n.getLocation (),
    												   ModifierSet.PUBLIC | ModifierSet.STATIC | ModifierSet.FINAL,
    												   null, type, variables);
    	return field;
    }

	/**
	 * Translate reference to enginetaint type to integer.
	 */
    public Object visit (ClassOrInterfaceType n, Object arg)
    throws Exception
    {
    	if (engineTaintNames.contains (n.name))
    	{
    		return new PrimitiveType (n.getLocation (), PrimitiveType.Primitive.Int);
    	}
    	else return super.visit (n, arg);
	}

	/**
	 * Translate set of taint literals to or of bitmap values.
	 */
    public Object visit (TaintLiteralExpr n, Object arg)
    throws Exception
    {
    	return taintNamesToExpression (n.getLocation (), n.values);
    }

    private Expression taintNamesToExpression (Location l, Collection<String> names)
    {
    	Expression expr = null;

    	for (String value : names)
    	{
    		Expression child = new NameExpr (l, value);
			if (expr == null)
			{
				expr = child;
			}
			else
			{
				expr = new BinaryExpr (l, expr, child, BinaryExpr.Operator.binOr);
			}
    	}

    	return new EnclosedExpr (l, expr);
    }
}
