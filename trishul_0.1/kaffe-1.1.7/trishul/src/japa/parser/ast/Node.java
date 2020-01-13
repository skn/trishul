/*
 * Created on 05/10/2006
 */
package japa.parser.ast;

import japa.parser.Location;
import japa.parser.ast.visitor.DumpVisitor;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class Node
{
    private final Location 	location;
    private String			commentAfter;
    private String			commentBefore;

    /**
     * This attribute can store additional information from semantic analysis.
     */
    public Object data;

    public Node(Location location) {
        this.location = location;
    }

    public <A> void accept(VoidVisitor<A> v, A arg) throws Exception {
        v.visit(this, arg);
    }

    public <R, A> R accept(GenericVisitor<R, A> v, A arg) throws Exception {
        return v.visit(this, arg);
    }

    @Override
    public final String toString() {
        DumpVisitor visitor = new DumpVisitor();
        try
        {
        	accept(visitor, null);
        }
        catch (Exception e) {/* Doesn't happen in DumpVisitor */}
        return visitor.getSource();
    }

    public final Location getLocation ()
    {
        return location;
    }

    public void setComment (String comment, boolean before)
    {
    	if (before) commentBefore = comment;
    	else commentAfter = comment;
	}

	public boolean hasComment (boolean before)
	{
    	return (before ? (commentBefore != null) : (commentAfter != null));
	}

	public String getComment (boolean before)
	{
    	return (before ? commentBefore : commentAfter);
	}

}
