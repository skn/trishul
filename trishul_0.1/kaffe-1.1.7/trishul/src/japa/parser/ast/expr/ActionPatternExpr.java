package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import trishul_p.ActionPattern;

public class ActionPatternExpr
extends Expression
{
    public ActionPattern 	pattern;

    public ActionPatternExpr (Location location, ActionPattern pattern)
    {
        super(location);
        this.pattern = pattern;
    }

    @Override
    public <A> void accept (VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit(this, arg);
    }

    @Override
    public <R, A> R accept (GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit(this, arg);
    }

}
