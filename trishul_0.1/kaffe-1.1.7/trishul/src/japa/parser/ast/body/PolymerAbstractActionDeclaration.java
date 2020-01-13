package japa.parser.ast.body;

import japa.parser.Location;
import japa.parser.ast.TypeParameter;
import japa.parser.ast.expr.AnnotationExpr;
import japa.parser.ast.type.ClassOrInterfaceType;
import java.util.List;

public class PolymerAbstractActionDeclaration
extends ClassOrInterfaceDeclaration
{
	public PolymerAbstractActionDeclaration (Location location, int modifiers,
											 List<AnnotationExpr> annotations, String name,
											 List<TypeParameter> typeParameters,
											 ClassOrInterfaceType ext,
											 List<ClassOrInterfaceType> implementsList,
											 List<BodyDeclaration> members)
	{
		super (location, modifiers, annotations, false, name, typeParameters, ext, implementsList, members);
	}
}