/*
 * Created on 18/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class TryStmt extends Statement {

    public BlockStmt tryBlock;

    public List<CatchClause> catchs;

    public BlockStmt finallyBlock;

    public TryStmt(Location location, BlockStmt tryBlock, List<CatchClause> catchs, BlockStmt finallyBlock) {
        super(location);
        this.tryBlock = tryBlock;
        this.catchs = catchs;
        this.finallyBlock = finallyBlock;
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
