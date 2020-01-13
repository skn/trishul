/*
 * Created on 05/10/2006
 */
package japa.parser.ast.type;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class ClassOrInterfaceType extends Type {

    public ClassOrInterfaceType scope;

    public String name;

    public List<Type> typeArgs;

    public ClassOrInterfaceType(Location location, ClassOrInterfaceType scope, String name, List<Type> typeArgs) {
        super(location);
        this.scope = scope;
        this.name = name;
        this.typeArgs = typeArgs;
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
