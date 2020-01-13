/*
 * Created on 04/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.LinkedList;
import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class SwitchEntryStmt extends Statement {

    public Expression label;

    public List<Statement> stmts;

    public SwitchEntryStmt(Location location, Expression label, List<Statement> stmts) {
        super(location);
        this.label = label;
        this.stmts = stmts;
    }

    public SwitchEntryStmt(Location location, Expression label, Statement stmt)
    {
        this (location, label, new LinkedList<Statement> ());
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
