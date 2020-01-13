/*
 * Created on 05/10/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.TypeParameter;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.ClassOrInterfaceType;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public class ClassOrInterfaceDeclaration extends TypeDeclaration {

    public List<AnnotationExpr> annotations;

    public boolean isInterface;

    public List<TypeParameter> typeParameters;

    public ClassOrInterfaceType extend;

    public List<ClassOrInterfaceType> implementsList;

    public ClassOrInterfaceDeclaration (Location location, int modifiers, List<AnnotationExpr> annotations,
    									boolean isInterface, String name, List<TypeParameter> typeParameters,
    									ClassOrInterfaceType extend, List<ClassOrInterfaceType> implementsList,
    									List<BodyDeclaration> members)
	{
        super(location, name, modifiers, members);
        this.annotations 	= annotations;
        this.isInterface 	= isInterface;
        this.typeParameters = typeParameters;
        this.extend 		= extend;
        this.implementsList = implementsList;
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
