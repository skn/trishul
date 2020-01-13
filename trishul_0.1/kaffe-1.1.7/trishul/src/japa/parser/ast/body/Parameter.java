/*
 * Created on 03/11/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.Node;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class Parameter extends Node {

    public int modifiers;

    public List<AnnotationExpr> annotations;

    public Type type;

    public boolean isVarArgs;

    public VariableDeclaratorId id;

    public Parameter(Location location, int modifiers, List<AnnotationExpr> annotations, Type type, boolean isVarArgs, VariableDeclaratorId id) {
        super(location);
        this.modifiers = modifiers;
        this.annotations = annotations;
        this.type = type;
        this.isVarArgs = isVarArgs;
        this.id = id;
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
