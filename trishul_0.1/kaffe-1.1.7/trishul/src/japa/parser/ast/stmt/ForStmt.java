/*
 * Created on 07/11/2006
 */
package japa.parser.ast.stmt;

import japa.parser.Location;
import japa.parser.ast.expr.Expression;
import japa.parser.ast.visitor.GenericVisitor;
import japa.parser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * @author Julio Vilmar Gesser
 */
public final class ForStmt extends Statement {

    public List<Expression> init;

    public List<Expression> update;

    public Expression iterable;

    public Statement body;

    public ForStmt(Location location, List<Expression> init, Expression iterable, List<Expression> update, Statement body) {
        super(location);
        this.iterable = iterable;
        this.init = init;
        this.update = update;
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
