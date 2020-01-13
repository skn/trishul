/*
 * Created on 05/10/2006
 */
package japa.parser.ast.body;

import japa.parser.Location;
import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class TypeDeclaration extends BodyDeclaration {

    public String name;

    public int modifiers;

    public List<BodyDeclaration> members;

    public TypeDeclaration(Location location, String name, int modifiers, List<BodyDeclaration> members) {
        super(location);
        this.name = name;
        this.modifiers = modifiers;
        this.members = members;
    }

}
