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
public final class UnaryExpr extends Expression {

    public static enum Operator {
        positive, // +
        negative, // -
        preIncrement, // ++
        preDecrement, // --
        not, // !
        inverse, // ~
        posIncrement, // ++
        posDecrement, // --
    }

    public Expression expr;

    public Operator op;

    public UnaryExpr(Location location, Expression expr, Operator op) {
        super(location);
        this.expr = expr;
        this.op = op;
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
