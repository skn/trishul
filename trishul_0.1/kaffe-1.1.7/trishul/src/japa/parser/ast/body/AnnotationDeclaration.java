/*
 * Created on 21/11/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class AnnotationDeclaration extends TypeDeclaration {

    public List<AnnotationExpr> annotations;

    public AnnotationDeclaration(Location location, int modifiers, List<AnnotationExpr> annotations, String name, List<BodyDeclaration> members) {
        super(location, name, modifiers, members);
        this.annotations = annotations;
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
