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
public final class WildcardType extends Type {

    public ReferenceType ext;

    public ReferenceType sup;

    public WildcardType(Location location, ReferenceType ext, ReferenceType sup) {
        super(location);
        assert ext == null || sup == null;
        this.ext = ext;
        this.sup = sup;
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
