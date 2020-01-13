package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.ClassOrInterfaceType;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

public final class EngineTaintDeclaration
extends TypeDeclaration
{
    public List<AnnotationExpr> annotations;

    public ClassOrInterfaceType extend;

    public List<EngineTaintConstantDeclaration> entries;

    public EngineTaintDeclaration (Location location, int modifiers, List<AnnotationExpr> annotations,
    							   String name, ClassOrInterfaceType extend,
    							   List<EngineTaintConstantDeclaration> entries)
	{
        super (location, name, modifiers, null);
        this.extend 	= extend;
        this.entries 	= entries;
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg)
    throws Exception
    {
        v.visit (this, arg);
    }

    @Override
    public <R, A> R accept (GenericVisitor<R, A> v, A arg)
    throws Exception
    {
        return v.visit (this, arg);
    }
}
