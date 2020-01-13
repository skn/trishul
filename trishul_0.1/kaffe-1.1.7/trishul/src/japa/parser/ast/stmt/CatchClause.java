/*
 * Created on 18/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.Node;
import japa.parser.ast.body.Parameter;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class CatchClause extends Node {

    public Parameter except;

    public BlockStmt catchBlock;

    public CatchClause(Location location, Parameter except, BlockStmt catchBlock) {
        super(location);
        this.except = except;
        this.catchBlock = catchBlock;
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
