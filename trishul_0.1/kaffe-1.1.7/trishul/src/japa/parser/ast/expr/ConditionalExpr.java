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
public final class ConditionalExpr extends Expression {

    public Expression condition;

    public Expression thenExpr;

    public Expression elseExpr;

    public ConditionalExpr(Location location, Expression condition, Expression thenExpr, Expression elseExpr) {
        super(location);
        this.condition = condition;
        this.thenExpr = thenExpr;
        this.elseExpr = elseExpr;
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
