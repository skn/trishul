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
public final class BinaryExpr extends Expression {

    public static enum Operator {
        or, // ||
        and, // &&
        binOr, // |
        binAnd, // &
        xor, // ^
        equals, // ==
        notEquals, // !=
        less, // <
        greater, // >
        lessEquals, // <=
        greaterEquals, // >=
        lShift, // <<
        rSignedShift, // >>
        rUnsignedShift, // >>>
        plus, // +
        minus, // -
        times, // *
        divide, // /
        remainder, // %
    }

    public Expression left;

    public Expression right;

    public Operator op;

    public BinaryExpr(Location location, Expression left, Expression right, Operator op) {
        super(location);
        this.left = left;
        this.right = right;
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
