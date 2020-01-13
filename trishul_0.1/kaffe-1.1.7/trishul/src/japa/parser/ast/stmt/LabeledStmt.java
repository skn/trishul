/*
 * Created on 04/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class LabeledStmt extends Statement {

    public String label;

    public Statement stmt;

    public LabeledStmt(Location location, String label, Statement stmt) {
        super(location);
        this.label = label;
        this.stmt = stmt;
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
