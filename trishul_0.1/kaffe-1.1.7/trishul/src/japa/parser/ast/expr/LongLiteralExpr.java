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
public class LongLiteralExpr extends StringLiteralExpr {

    public LongLiteralExpr(Location location, String value) {
        super(location, value);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) throws Exception {
        v.visit(this, arg);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) throws Exception {
        return v.visit(this, arg);
    }

    public final boolean isMinValue() {
        return value != null && //
                value.length() == 20 && //
                value.startsWith("9223372036854775808") && //
                (value.charAt(19) == 'L' || value.charAt(19) == 'l');
    }
}
