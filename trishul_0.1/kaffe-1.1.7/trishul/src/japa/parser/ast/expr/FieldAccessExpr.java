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
public final class FieldAccessExpr extends Expression {

    public Expression scope;

    public List<Type> typeArgs;

    public String field;

    public FieldAccessExpr(Location location, Expression scope, List<Type> typeArgs, String field) {
        super(location);
        this.scope = scope;
        this.typeArgs = typeArgs;
        this.field = field;
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
