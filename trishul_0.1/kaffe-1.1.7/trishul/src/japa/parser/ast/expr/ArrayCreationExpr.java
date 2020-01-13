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
public final class ArrayCreationExpr extends Expression {

    public Type type;

    public List<Type> typeArgs;

    public int arrayCount;

    public ArrayInitializerExpr initializer;

    public List<Expression> dimensions;

    public ArrayCreationExpr(Location location, Type type, List<Type> typeArgs, int arrayCount, ArrayInitializerExpr initializer) {
        super(location);
        this.type = type;
        this.typeArgs = typeArgs;
        this.arrayCount = arrayCount;
        this.initializer = initializer;
        this.dimensions = null;
    }

    public ArrayCreationExpr(Location location, Type type, List<Type> typeArgs, List<Expression> dimensions, int arrayCount) {
        super(location);
        this.type = type;
        this.typeArgs = typeArgs;
        this.arrayCount = arrayCount;
        this.dimensions = dimensions;
        this.initializer = null;
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
