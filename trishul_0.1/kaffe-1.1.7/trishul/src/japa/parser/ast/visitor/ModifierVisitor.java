package japa.parser.ast.visitor;

import japa.parser.Location;
import japa.parser.ast.*;
import japa.parser.ast.body.*;
import japa.parser.ast.expr.*;
import japa.parser.ast.stmt.*;
import japa.parser.ast.type.*;

import java.util.List;
import java.util.LinkedList;

public class ModifierVisitor
implements GenericVisitor<Object, Object>
{
	private boolean haveErrors = false;

	protected void error (Node n, String msg)
	{
		haveErrors = true;
		Location loc = n.getLocation ();
		System.err.println (loc.getFile () + " (" + loc.getLine () + ", " + loc.getColumn () + "): " + msg);
	}

	public boolean haveErrors ()
	{
		return haveErrors;
	}

    public Object visit(Node n, Object arg) throws Exception
    {
    	return n.accept (this, arg);
    }

    //- Compilation Unit ----------------------------------

	@SuppressWarnings("unchecked")
	private <A extends Node> List<A> handleList (List<A> list, Object arg) throws Exception
	{
		if (list != null)
		{
			LinkedList<A> newList = new LinkedList<A> ();

			for (A a: list)
			{
				A newA = (A) visit (a, arg);
				if (newA != null)
					newList.addLast (newA);
			}

			if (!newList.isEmpty ())
				return newList;
		}
		return null;
	}

	@SuppressWarnings("unchecked")
	private <A extends Node> A handleObject (A obj, Object arg) throws Exception
	{
		if (obj == null)
			return null;
		return (A) visit (obj, arg);
	}

    public Object visit (CompilationUnit n, Object arg)
    throws Exception
    {
		n.imports 	= handleList (n.imports, arg);
		n.types 	= handleList (n.types, arg);
    	return n;
    }

    public Object visit (ImportDeclaration n, Object arg)
    throws Exception
    {
    	n.name = handleObject (n.name, arg);
    	return n;
    }

    public Object visit (TypeParameter n, Object arg)
    throws Exception
    {
    	n.typeBound = handleList (n.typeBound, arg);
    	return n;
    }

    //- Body ----------------------------------------------

    public Object visit (ClassOrInterfaceDeclaration n, Object arg) throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, 	arg);
    	n.typeParameters	= handleList 	(n.typeParameters, 	arg);
    	n.extend			= handleObject 	(n.extend,	 		arg);
    	n.implementsList	= handleList 	(n.implementsList, 	arg);
    	n.members			= handleList 	(n.members, 		arg);
    	return n;
    }

    public Object visit (EnumDeclaration n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.implementsList	= handleList 	(n.implementsList, arg);
    	n.entries			= handleList 	(n.entries, arg);
    	n.members			= handleList 	(n.members, arg);
    	return n;
    }

    public Object visit (EmptyTypeDeclaration n, Object arg)
    throws Exception
    {
    	n.members			= handleList 	(n.members, arg);
    	return n;
    }

    public Object visit (EnumConstantDeclaration n, Object arg)
    throws Exception
    {
    	n.classBody			= handleList 	(n.classBody, 	arg);
    	n.args				= handleList 	(n.args, 		arg);
    	return n;
    }

    public Object visit (AnnotationDeclaration n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.members			= handleList 	(n.members, arg);
    	return n;
    }

    public Object visit (AnnotationMemberDeclaration n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.type				= handleObject 	(n.type, arg);
    	n.defaultValue		= handleObject 	(n.defaultValue, arg);
    	return n;
    }

    public Object visit (FieldDeclaration n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.type				= handleObject 	(n.type, arg);
    	n.variables			= handleList 	(n.variables, arg);
    	return n;
    }

    public Object visit (VariableDeclarator n, Object arg)
    throws Exception
    {
    	n.id				= handleObject 	(n.id, arg);
    	n.init				= handleObject 	(n.init, arg);
    	return n;
    }

    public Object visit (VariableDeclaratorId n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (ConstructorDeclaration n, Object arg)
    throws Exception
    {
   		n.annotations		= handleList 	(n.annotations, arg);
    	n.typeParameters	= handleList 	(n.typeParameters, arg);
    	n.parameters		= handleList 	(n.parameters, arg);
    	n.throws_			= handleList 	(n.throws_, arg);
    	n.block				= handleObject 	(n.block, arg);
     	return n;
    }

    public Object visit (MethodDeclaration n, Object arg)
    throws Exception
    {
   		n.annotations		= handleList 	(n.annotations, arg);
    	n.typeParameters	= handleList 	(n.typeParameters, arg);
    	n.type				= handleObject 	(n.type, arg);
    	n.parameters		= handleList 	(n.parameters, arg);
    	n.throws_			= handleList 	(n.throws_, arg);
    	n.block				= handleObject 	(n.block, arg);
     	return n;
    }

    public Object visit (Parameter n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.type				= handleObject 	(n.type, arg);
    	n.id				= handleObject 	(n.id, arg);
    	return n;
    }

    public Object visit (EmptyMemberDeclaration n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (InitializerDeclaration n, Object arg)
    throws Exception
    {
     	n.block				= handleObject 	(n.block, arg);
	   	return n;
    }

    public Object visit (TrishulTaintDeclaration n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (EngineTaintDeclaration n, Object arg)
    throws Exception
    {
    	n.annotations		= handleList 	(n.annotations, arg);
    	n.extend			= handleObject 	(n.extend, arg);
    	n.entries			= handleList 	(n.entries, arg);
    	n.members			= handleList 	(n.members, arg);
    	return n;
    }

    public Object visit (EngineTaintConstantDeclaration n, Object arg)
    throws Exception
    {
    	return n;
    }

    //- Type ----------------------------------------------

    public Object visit (ClassOrInterfaceType n, Object arg)
    throws Exception
    {
    	n.scope				= handleObject 	(n.scope, arg);
    	n.typeArgs			= handleList 	(n.typeArgs, arg);
    	return n;
    }

    public Object visit (PrimitiveType n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (ReferenceType n, Object arg)
    throws Exception
    {
    	n.type				= handleObject 	(n.type, arg);
    	return n;
    }

    public Object visit (VoidType n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (WildcardType n, Object arg)
    throws Exception
    {
    	n.ext				= handleObject 	(n.ext, arg);
    	n.sup				= handleObject 	(n.sup, arg);
    	return n;
    }

    //- Expression ----------------------------------------

    public Object visit (ArrayAccessExpr n, Object arg)
    throws Exception
	{
    	n.name				= handleObject 	(n.name, arg);
    	n.index				= handleObject 	(n.index, arg);
		return n;
	}

    public Object visit (ArrayCreationExpr n, Object arg)
    throws Exception
	{
    	n.type				= handleObject 	(n.type, arg);
    	n.typeArgs			= handleList 	(n.typeArgs, arg);
    	n.initializer		= handleObject 	(n.initializer, arg);
    	n.dimensions		= handleList 	(n.dimensions, arg);
		return n;
	}

    public Object visit (ArrayInitializerExpr n, Object arg)
    throws Exception
	{
    	n.values			= handleList 	(n.values, arg);
		return n;
	}

    public Object visit (AssignExpr n, Object arg)
    throws Exception
	{
    	n.target			= handleObject 	(n.target, arg);
    	n.value				= handleObject 	(n.value, arg);
		return n;
	}

    public Object visit (BinaryExpr n, Object arg)
    throws Exception
	{
    	n.left				= handleObject 	(n.left, arg);
    	n.right				= handleObject 	(n.right, arg);
		return n;
	}

    public Object visit (CastExpr n, Object arg)
    throws Exception
	{
    	n.expr				= handleObject	(n.expr, arg);
    	n.type				= handleObject	(n.type, arg);
		return n;
	}

    public Object visit (ClassExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (ConditionalExpr n, Object arg)
    throws Exception
	{
    	n.condition			= handleObject	(n.condition, arg);
    	n.thenExpr			= handleObject	(n.thenExpr, arg);
    	n.elseExpr			= handleObject	(n.elseExpr, arg);
		return n;
	}

    public Object visit (EnclosedExpr n, Object arg)
    throws Exception
	{
    	n.inner				= handleObject	(n.inner, arg);
		return n;
	}

    public Object visit (FieldAccessExpr n, Object arg)
    throws Exception
    {
    	n.scope				= handleObject	(n.scope, arg);
    	n.typeArgs			= handleList 	(n.typeArgs, arg);
    	return n;
    }

    public Object visit (InstanceOfExpr n, Object arg)
    throws Exception
	{
    	n.expr				= handleObject	(n.expr, arg);
    	n.type				= handleObject	(n.type, arg);
		return n;
	}

    public Object visit (StringLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (IntegerLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (LongLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (IntegerLiteralMinValueExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (LongLiteralMinValueExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (CharLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (DoubleLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (BooleanLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (NullLiteralExpr n, Object arg)
    throws Exception
    {
    	return n;
   	}

    public Object visit (MethodCallExpr n, Object arg)
    throws Exception
    {
    	n.scope		= handleObject	(n.scope, arg);
    	n.typeArgs	= handleList 	(n.typeArgs, arg);
    	n.args		= handleList	(n.args, arg);
    	return n;
    }

    public Object visit (NameExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (ObjectCreationExpr n, Object arg)
    throws Exception
	{
    	n.scope					= handleObject	(n.scope, arg);
    	n.type					= handleObject	(n.type, arg);
    	n.typeArgs				= handleList 	(n.typeArgs, arg);
    	n.args					= handleList 	(n.args, arg);
    	n.anonymousClassBody	= handleList 	(n.anonymousClassBody, arg);
		return n;
	}

    public Object visit (QualifiedNameExpr n, Object arg)
    throws Exception
	{
    	n.qualifier		= handleObject	(n.qualifier, arg);
		return n;
	}

    public Object visit (SuperMemberAccessExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (ThisExpr n, Object arg)
    throws Exception
    {
    	n.classExpr 	= handleObject (n.classExpr, arg);
    	return n;
    }

    public Object visit (SuperExpr n, Object arg)
    throws Exception
	{
    	n.classExpr 	= handleObject (n.classExpr, arg);
		return n;
	}

    public Object visit (UnaryExpr n, Object arg)
    throws Exception
	{
    	n.expr			= handleObject	(n.expr, arg);
		return n;
	}

    public Object visit (VariableDeclarationExpr n, Object arg)
    throws Exception
	{
    	n.annotations	= handleList 	(n.annotations, arg);
    	n.type			= handleObject	(n.type, arg);
    	n.vars			= handleList	(n.vars, arg);
		return n;
	}

    public Object visit (MarkerAnnotationExpr n, Object arg)
    throws Exception
	{
    	n.name			= handleObject	(n.name, arg);
		return n;
	}

    public Object visit (SingleMemberAnnotationExpr n, Object arg)
    throws Exception
	{
    	n.name			= handleObject	(n.name, arg);
    	n.memberValue	= handleObject	(n.memberValue, arg);
		return n;
	}

    public Object visit (NormalAnnotationExpr n, Object arg)
    throws Exception
	{
    	n.name			= handleObject	(n.name, arg);
    	n.pairs			= handleList 	(n.pairs, arg);
		return n;
	}

    public Object visit (MemberValuePair n, Object arg)
    throws Exception
	{
    	n.value			= handleObject	(n.value, arg);
		return n;
	}

    public Object visit (ActionPatternExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (TaintPatternExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (TaintExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    public Object visit (TaintLiteralExpr n, Object arg)
    throws Exception
	{
		return n;
	}

    //- Statements ----------------------------------------

    public Object visit(ExplicitConstructorInvocationStmt n, Object arg) throws Exception
    {
    	n.expr = handleObject 	(n.expr, arg);
    	n.args = handleList 	(n.args, arg);
    	return n;
    }

    public Object visit (TypeDeclarationStmt n, Object arg)
    throws Exception
    {
    	n.typeDecl	= handleObject (n.typeDecl, arg);
    	return n;
    }

    public Object visit (AssertStmt n, Object arg)
    throws Exception
    {
    	n.check = handleObject (n.check, arg);
    	n.msg   = handleObject (n.msg, arg);
    	return n;
    }

    public Object visit (BlockStmt n, Object arg)
    throws Exception
    {
    	n.stmts = handleList (n.stmts, arg);
    	return n;
    }

    public Object visit (LabeledStmt n, Object arg)
    throws Exception
    {
    	n.stmt = handleObject (n.stmt, arg);
    	return n;
    }

    public Object visit (EmptyStmt n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (ExpressionStmt n, Object arg)
    throws Exception
    {
    	n.expr = handleObject (n.expr, arg);
    	return n;
    }

    public Object visit (SwitchStmt n, Object arg)
    throws Exception
    {
    	n.selector = handleObject (n.selector, arg);
    	n.entries  = handleList   (n.entries, arg);
    	return n;
    }

    public Object visit (SwitchEntryStmt n, Object arg)
    throws Exception
    {
    	n.label = handleObject (n.label, arg);
    	n.stmts = handleList   (n.stmts, arg);
    	return n;
    }

    public Object visit (BreakStmt n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (ReturnStmt n, Object arg)
    throws Exception
    {
    	n.expr = handleObject (n.expr, arg);
    	return n;
    }

    public Object visit (IfStmt n, Object arg)
    throws Exception
    {
    	n.condition = handleObject (n.condition, arg);
    	n.thenStmt  = handleObject (n.thenStmt, arg);
    	n.elseStmt  = handleObject (n.elseStmt, arg);
    	return n;
    }

    public Object visit (WhileStmt n, Object arg)
    throws Exception
    {
    	n.condition = handleObject (n.condition, arg);
    	n.body  	= handleObject (n.body, arg);
    	return n;
    }

    public Object visit (ContinueStmt n, Object arg)
    throws Exception
    {
    	return n;
    }

    public Object visit (DoStmt n, Object arg)
    throws Exception
    {
    	n.body 		= handleObject (n.body, arg);
    	n.condition = handleObject (n.condition, arg);
    	return n;
    }

    public Object visit (ForeachStmt n, Object arg)
    throws Exception
    {
     	n.var		= handleObject (n.var, arg);
     	n.iterable	= handleObject (n.iterable, arg);
     	n.body		= handleObject (n.body, arg);
    	return n;
    }

    public Object visit (ForStmt n, Object arg)
    throws Exception
    {
    	n.init		= handleList   (n.init, arg);
    	n.update	= handleList   (n.update, arg);
     	n.iterable	= handleObject (n.iterable, arg);
     	n.body		= handleObject (n.body, arg);
	   	return n;
    }

    public Object visit (ThrowStmt n, Object arg)
    throws Exception
    {
    	n.expr	= handleObject (n.expr, arg);
    	return n;
    }

    public Object visit (SynchronizedStmt n, Object arg)
    throws Exception
    {
    	n.expr	= handleObject (n.expr, arg);
    	n.block	= handleObject (n.block, arg);
    	return n;
    }

    public Object visit (TryStmt n, Object arg)
    throws Exception
    {
    	n.tryBlock		= handleObject (n.tryBlock, arg);
    	n.catchs		= handleList   (n.catchs, arg);
    	n.finallyBlock	= handleObject (n.finallyBlock, arg);
    	return n;
    }

    public Object visit (CatchClause n, Object arg)
    throws Exception
    {
    	n.except 	 = handleObject (n.except, arg);
    	n.catchBlock = handleObject (n.catchBlock, arg);
    	return n;
    }

    public Object visit (PolymerActionSwitchStmt n, Object arg)
    throws Exception
    {
    	n.selector = handleObject (n.selector, arg);
    	n.entries  = handleList   (n.entries, arg);
    	return n;
    }

    public Object visit (PolymerActionSwitchEntryStmt n, Object arg)
    throws Exception
    {
    	n.label = handleObject (n.label, arg);
    	n.stmts = handleList   (n.stmts, arg);
    	return n;
    }
}
