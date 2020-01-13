package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

public final class EngineTaintConstantDeclaration
extends BodyDeclaration
{
    public String name;

    public EngineTaintConstantDeclaration (Location location, String name)
    {
        super(location);
        this.name = name;
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
