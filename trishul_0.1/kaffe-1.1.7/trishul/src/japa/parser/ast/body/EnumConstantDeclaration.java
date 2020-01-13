/*
 * Created on 05/11/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class EnumConstantDeclaration extends BodyDeclaration {

    public String name;

    public List<Expression> args;

    public List<BodyDeclaration> classBody;

    public EnumConstantDeclaration(Location location, String name, List<Expression> args, List<BodyDeclaration> classBody) {
        super(location);
        this.name = name;
        this.args = args;
        this.classBody = classBody;
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
