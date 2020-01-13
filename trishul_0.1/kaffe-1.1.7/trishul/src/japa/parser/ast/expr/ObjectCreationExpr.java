/*
 * Created on 05/10/2006
 */
package japa.parser.ast.expr;

import japa.parser.Location;
import japa.parser.ast.body.BodyDeclaration;
import japa.parser.ast.type.ClassOrInterfaceType;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class ObjectCreationExpr extends Expression {

    public Expression scope;

    public ClassOrInterfaceType type;

    public List<Type> typeArgs;

    public List<Expression> args;

    public List<BodyDeclaration> anonymousClassBody;

    public ObjectCreationExpr(Location location, Expression scope, ClassOrInterfaceType type, List<Type> typeArgs, List<Expression> args, List<BodyDeclaration> anonymousBody) {
        super(location);
        this.scope = scope;
        this.type = type;
        this.typeArgs = typeArgs;
        this.args = args;
        this.anonymousClassBody = anonymousBody;
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
