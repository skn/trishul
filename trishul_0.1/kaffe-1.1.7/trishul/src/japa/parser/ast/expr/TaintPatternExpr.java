package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import trishul_p.TaintPattern;

public final class TaintPatternExpr
extends LiteralExpr
{
	public TaintPattern taintPattern;

    public TaintPatternExpr (Location location, TaintPattern taintPattern)
    {
        super (location);
        this.taintPattern = taintPattern;
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit (this, arg);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit (this, arg);
    }
}
