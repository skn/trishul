/*
 * Created on 05/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class ArrayAccessExpr extends Expression {

    public Expression name;

    public Expression index;

    public ArrayAccessExpr(Location location, Expression name, Expression index) {
        super(location);
        this.name = name;
        this.index = index;
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
