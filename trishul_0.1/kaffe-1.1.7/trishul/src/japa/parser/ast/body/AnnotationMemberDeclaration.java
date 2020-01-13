/*
 * Created on 21/11/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.type.Type;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class AnnotationMemberDeclaration extends BodyDeclaration {

    public int modifiers;

    public List<AnnotationExpr> annotations;

    public Type type;

    public String name;

    public Expression defaultValue;

    public AnnotationMemberDeclaration(Location location, int modifiers, List<AnnotationExpr> annotations, Type type, String name, Expression defaultValue) {
        super(location);
        this.modifiers = modifiers;
        this.annotations = annotations;
        this.type = type;
        this.name = name;
        this.defaultValue = defaultValue;
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
