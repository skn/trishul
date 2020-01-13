/*
 * Created on 07/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.expr.VariableDeclarationExpr;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class ForeachStmt extends Statement {

    public VariableDeclarationExpr var;

    public Expression iterable;

    public Statement body;

    public ForeachStmt(Location location, VariableDeclarationExpr var, Expression iterable, Statement body) {
        super(location);
        this.var = var;
        this.iterable = iterable;
        this.body = body;
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
