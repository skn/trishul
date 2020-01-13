package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import java.util.HashSet;

public final class TaintLiteralExpr
extends LiteralExpr
{
	public HashSet<String>	values;

    public TaintLiteralExpr (Location location, HashSet<String> values)
    {
        super (location);
        this.values = values;
    }

    public <A> void accept (VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit (this, arg);
    }

    public <R, A> R accept (GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit (this, arg);
    }
}
