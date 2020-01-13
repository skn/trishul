/*
 * Created on 05/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
/**
 * @author Julio Vilmar Gesser
 */
public abstract class LiteralExpr extends Expression {

    public LiteralExpr(Location location) {
        super(location);
    }
}
