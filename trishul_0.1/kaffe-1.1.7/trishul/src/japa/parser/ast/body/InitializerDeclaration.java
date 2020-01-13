/*
 * Created on 07/11/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.stmt.BlockStmt;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class InitializerDeclaration extends BodyDeclaration {

    public BlockStmt block;

    public InitializerDeclaration(Location location, BlockStmt block) {
        super(location);
        this.block = block;
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
