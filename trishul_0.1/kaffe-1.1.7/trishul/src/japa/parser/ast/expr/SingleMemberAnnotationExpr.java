/*
 * Created on 21/11/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class SingleMemberAnnotationExpr extends AnnotationExpr {

    public NameExpr name;

    public Expression memberValue;

    public SingleMemberAnnotationExpr(Location location, NameExpr name, Expression memberValue) {
        super(location);
        this.name = name;
        this.memberValue = memberValue;
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
