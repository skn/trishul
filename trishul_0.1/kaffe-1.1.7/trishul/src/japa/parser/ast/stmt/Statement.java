/*
 * Created on 03/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.Node;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class Statement extends Node {

    public Statement(Location location) {
        super(location);
    }

}
