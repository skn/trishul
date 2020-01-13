/*
 * Created on 05/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class MethodCallExpr extends Expression {

    public Expression scope;

    public List<Type> typeArgs;

    public String name;

    public List<Expression> args;

    public MethodCallExpr(Location location, Expression scope, List<Type> typeArgs, String name, List<Expression> args) {
        super(location);
        this.scope = scope;
        this.typeArgs = typeArgs;
        this.name = name;
        this.args = args;
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
