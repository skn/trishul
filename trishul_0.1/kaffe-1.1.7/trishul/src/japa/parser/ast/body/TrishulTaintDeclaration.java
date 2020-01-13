package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.TypeParameter;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.ClassOrInterfaceType;
import java.util.List;

public class TrishulTaintDeclaration
extends ClassOrInterfaceDeclaration
{
    public TrishulTaintDeclaration (Location location, int modifiers, List<AnnotationExpr> annotations,
    									String name, List<TypeParameter> typeParameters,
    									List<ClassOrInterfaceType> implementsList,
    									List<BodyDeclaration> members)
	{
        super(location, modifiers, annotations, false, name, typeParameters, null, implementsList, members);
    }
}
