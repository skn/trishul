/*
 * Created on 10/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.Node;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class Expression extends Node {

    public Expression(Location location) {
        super(location);
    }

}
