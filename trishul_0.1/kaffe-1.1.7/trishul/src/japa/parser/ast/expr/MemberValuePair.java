/*
 * Created on 21/11/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.Node;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class MemberValuePair extends Node {

    public String name;

    public Expression value;

    public MemberValuePair(Location location, String name, Expression value) {
        super(location);
        this.name = name;
        this.value = value;
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
