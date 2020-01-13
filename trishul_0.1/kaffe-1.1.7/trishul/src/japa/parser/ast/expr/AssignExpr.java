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
public final class AssignExpr extends Expression {

    public static enum Operator {
        assign, // =
        plus, // +=
        minus, // -=
        star, // *=
        slash, // /=
        and, // &=
        or, // |=
        xor, // ^=
        rem, // %=
        lShift, // <<=
        rSignedShift, // >>=
        rUnsignedShift, // >>>=
    }

    public Expression target;

    public Expression value;

    public Operator op;

    public AssignExpr(Location location, Expression target, Expression value, Operator op) {
        super(location);
        this.target = target;
        this.value = value;
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
