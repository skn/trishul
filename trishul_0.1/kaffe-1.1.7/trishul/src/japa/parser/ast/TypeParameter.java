/*
 * Created on 05/10/2006
 */
package japa.parser.ast;

import japa.parser.Location;
import japa.parser.ast.type.ClassOrInterfaceType;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class TypeParameter extends Node {

    public String name;

    public List<ClassOrInterfaceType> typeBound;

    public TypeParameter(Location location, String name, List<ClassOrInterfaceType> typeBound) {
        super(location);
        this.name = name;
        this.typeBound = typeBound;
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
