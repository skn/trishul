package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;
import trishul_p.Taint;

public final class TaintExpr
extends LiteralExpr
{
	public Taint taint;

    public TaintExpr (Location location, Taint taint)
    {
        super (location);
        this.taint = taint;
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
