/*
 * Created on 05/10/2006
 */
package japa.parser.ast.type;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class ReferenceType extends Type {

    public Type type;

    public int arrayCount;

    public ReferenceType(Location location, Type type, int arrayCount) {
        super(location);
        this.type = type;
        this.arrayCount = arrayCount;
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
