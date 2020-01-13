/*
 * Created on 20/01/2007
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class EmptyTypeDeclaration extends TypeDeclaration {

    public EmptyTypeDeclaration(Location location) {
        super(location, null, 0, null);
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
