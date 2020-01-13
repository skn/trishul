/*
 * Created on 05/10/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.LinkedList;
import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class FieldDeclaration extends BodyDeclaration {

    public int modifiers;

    public List<AnnotationExpr> annotations;

    public Type type;

    public List<VariableDeclarator> variables;

    public FieldDeclaration (Location location, int modifiers, List<AnnotationExpr> annotations,
    						 Type type, VariableDeclarator variable)
	{
		this (location, modifiers, annotations, type, new LinkedList<VariableDeclarator> ());
		variables.add (variable);
	}

    public FieldDeclaration (Location location, int modifiers, List<AnnotationExpr> annotations,
    						 Type type, List<VariableDeclarator> variables)
	{
        super(location);
        this.modifiers = modifiers;
        this.annotations = annotations;
        this.type = type;
        this.variables = variables;
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) throws Exception {
        v.visit(this, arg);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) throws Exception {
        return v.visit(this, arg);
    }
}
