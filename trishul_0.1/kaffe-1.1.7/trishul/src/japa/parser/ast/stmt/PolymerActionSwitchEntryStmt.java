package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.expr.ActionPatternExpr;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.List;
import trishul_p.ActionPattern;

public class PolymerActionSwitchEntryStmt
extends Statement
{
    public ActionPatternExpr label;

    public List<Statement> stmts;

    public PolymerActionSwitchEntryStmt (Location location, ActionPatternExpr label, List<Statement> stmts)
    {
        super(location);
        this.label = label;
        this.stmts = stmts;
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit(this, arg);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit(this, arg);
    }
}
