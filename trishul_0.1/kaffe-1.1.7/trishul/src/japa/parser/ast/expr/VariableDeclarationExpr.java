/*
 * Created on 03/11/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.body.VariableDeclarator;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.LinkedList;
import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class VariableDeclarationExpr extends Expression {

    public int modifiers;

    public List<AnnotationExpr> annotations;

    public Type type;

    public List<VariableDeclarator> vars;

    public VariableDeclarationExpr (Location location, int modifiers, List<AnnotationExpr> annotations,
    								Type type, List<VariableDeclarator> vars)
	{
        super(location);
        this.modifiers = modifiers;
        this.annotations = annotations;
        this.type = type;
        this.vars = vars;
    }

    public VariableDeclarationExpr (Location location, int modifiers, List<AnnotationExpr> annotations,
    								Type type, VariableDeclarator var)
	{
        super(location);
        this.modifiers = modifiers;
        this.annotations = annotations;
        this.type = type;
        this.vars = new LinkedList<VariableDeclarator> ();
        this.vars.add (var);
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
