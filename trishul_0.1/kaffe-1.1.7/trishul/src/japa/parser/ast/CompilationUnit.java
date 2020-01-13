/*
 * Created on 05/10/2006
 */
package japa.parser.ast;

import japa.parser.Location;
import japa.parser.ast.body.TypeDeclaration;
import japa.parser.ast.body.EngineTaintDeclaration;
import japa.parser.ast.expr.NameExpr;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class CompilationUnit extends Node {

    public NameExpr pakage;

    public List<ImportDeclaration> imports;

    public List<TypeDeclaration> types;

	/**
	 * List of all enginetaint declarations, collected by the parser.
	 */
    public List<EngineTaintDeclaration> engineTaints;

    public CompilationUnit (Location location, NameExpr pakage, List<ImportDeclaration> imports,
    						List<TypeDeclaration> types, List<EngineTaintDeclaration> engineTaints)
	{
        super(location);
        this.pakage 		= pakage;
        this.imports 		= imports;
        this.types 			= types;
        this.engineTaints	= engineTaints;
    }

    public <A> void accept (VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit (this, arg);
    }

    public <R, A> R accept (GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit (this, arg);
    }

}
