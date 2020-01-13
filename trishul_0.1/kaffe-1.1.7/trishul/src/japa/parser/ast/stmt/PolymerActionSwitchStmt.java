package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.List;

public final class PolymerActionSwitchStmt
extends Statement
{

    public Expression selector;

    public List<PolymerActionSwitchEntryStmt> entries;

    public PolymerActionSwitchStmt (Location location, Expression selector,
    								List<PolymerActionSwitchEntryStmt> entries)
	{
        super(location);
        this.selector = selector;
        this.entries = entries;
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
