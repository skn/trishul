/*
 * Created on 04/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.LinkedList;
import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class BlockStmt extends Statement {

    public List<Statement> stmts;

    public BlockStmt(Location location, List<Statement> stmts) {
        super(location);
        this.stmts = stmts;
    }

    public BlockStmt(Location location, Statement stmt)
    {
        this (location, new LinkedList<Statement> ());
        stmts.add (stmt);
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
