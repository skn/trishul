/*
 * Created on 05/10/2006
 */
package japa.parser.ast;

import japa.parser.Location;
import japa.parser.ast.expr.NameExpr;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class ImportDeclaration extends Node {

    public NameExpr name;

    public boolean isStatic;

    public boolean isAsterisk;

    public ImportDeclaration(Location location, NameExpr name, boolean isStatic, boolean isAsterisk) {
        super(location);
        this.name = name;
        this.isStatic = isStatic;
        this.isAsterisk = isAsterisk;
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
