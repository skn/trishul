/*
 * Created on 05/10/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.Node;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class VariableDeclarator extends Node {

    public VariableDeclaratorId id;

    public Expression init;

    public VariableDeclarator (Location location, VariableDeclaratorId id, Expression init) {
        super(location);
        this.id = id;
        this.init = init;
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
