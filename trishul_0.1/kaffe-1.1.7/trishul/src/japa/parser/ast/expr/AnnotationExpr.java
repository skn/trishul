/*
 * Created on 21/11/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class AnnotationExpr extends Expression {

    public AnnotationExpr(Location location) {
        super(location);
    }

}
