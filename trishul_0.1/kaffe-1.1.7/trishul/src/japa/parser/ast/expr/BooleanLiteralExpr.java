/*
 * Created on 02/03/2007
 */
package japa.parser.ast.expr;
import japa.parser.Location;

import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class BooleanLiteralExpr extends LiteralExpr {

    public Boolean value;

    public BooleanLiteralExpr(Location location, Boolean value) {
        super(location);
        this.value = value;
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
