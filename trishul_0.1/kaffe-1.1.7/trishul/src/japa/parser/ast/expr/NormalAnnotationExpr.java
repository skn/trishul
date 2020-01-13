/*
 * Created on 21/11/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class NormalAnnotationExpr extends AnnotationExpr {

    public NameExpr name;

    public List<MemberValuePair> pairs;

    public NormalAnnotationExpr(Location location, NameExpr name, List<MemberValuePair> pairs) {
        super(location);
        this.name = name;
        this.pairs = pairs;
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
