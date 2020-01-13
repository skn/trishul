/*
 * Created on 05/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class InstanceOfExpr extends Expression {

    public Expression expr;

    public Type type;

    public InstanceOfExpr(Location location, Expression expr, Type type) {
        super(location);
        this.expr = expr;
        this.type = type;
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
