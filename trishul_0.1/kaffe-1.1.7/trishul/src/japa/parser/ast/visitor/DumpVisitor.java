/*
 * Created on 05/10/2006
 */
package japa.parser.ast.visitor;

import japa.parser.Location;
import japa.parser.ast.CompilationUnit;
import japa.parser.ast.ImportDeclaration;
import japa.parser.ast.Node;
import japa.parser.ast.TypeParameter;
import japa.parser.ast.body.*;
import japa.parser.ast.expr.*;
import japa.parser.ast.stmt.*;
import japa.parser.ast.type.*;
import java.util.Iterator;
import java.util.List;
import trishul_p.Taint;

/**
 * @author Julio Vilmar Gesser
 */

public class DumpVisitor implements VoidVisitor<Object> {

    protected final SourcePrinter printer = new SourcePrinter();

    public String getSource() {
        return printer.getSource();
    }

    private void printModifiers(int modifiers) {
        if (ModifierSet.isPrivate(modifiers)) {
            printer.print("private ");
        }
        if (ModifierSet.isProtected(modifiers)) {
            printer.print("protected ");
        }
        if (ModifierSet.isPublic(modifiers)) {
            printer.print("public ");
        }
        if (ModifierSet.isAbstract(modifiers)) {
            printer.print("abstract ");
        }
        if (ModifierSet.isFinal(modifiers)) {
            printer.print("final ");
        }
        if (ModifierSet.isNative(modifiers)) {
            printer.print("native ");
        }
        if (ModifierSet.isStatic(modifiers)) {
            printer.print("static ");
        }
        if (ModifierSet.isStrictfp(modifiers)) {
            printer.print("strictfp ");
        }
        if (ModifierSet.isSynchronized(modifiers)) {
            printer.print("synchronized ");
        }
        if (ModifierSet.isTransient(modifiers)) {
            printer.print("transient ");
        }
        if (ModifierSet.isVolatile(modifiers)) {
            printer.print("volatile ");
        }
    }

    private void printMembers(List<BodyDeclaration> members, Object arg) throws Exception {
        for (BodyDeclaration member : members) {
            printer.println();
            member.accept(this, arg);
            printer.println();
        }
    }

    private void printMemberAnnotations(List<AnnotationExpr> annotations, Object arg) throws Exception {
        if (annotations != null) {
            for (AnnotationExpr a : annotations) {
                a.accept(this, arg);
                printer.println();
            }
        }
    }

    private void printAnnotations(List<AnnotationExpr> annotations, Object arg) throws Exception {
        if (annotations != null) {
            for (AnnotationExpr a : annotations) {
                a.accept(this, arg);
                printer.print(" ");
            }
        }
    }

    public void visit(Node n, Object arg)
    throws Exception
    {
        throw new IllegalStateException(n.getClass().getName());
    }

    public void visit(CompilationUnit n, Object arg)
    throws Exception
    {
        if (n.pakage != null) {
            printer.print("package ");
            n.pakage.accept(this, arg);
            printer.println(";");
            printer.println();
        }
        if (n.imports != null) {
            for (ImportDeclaration i : n.imports) {
                i.accept(this, arg);
            }
            printer.println();
        }
        if (n.types != null) {
            for (TypeDeclaration i : n.types) {
                i.accept(this, arg);
                printer.println();
            }
        }
    }

    public void visit(NameExpr n, Object arg)
    throws Exception
    {
        printer.print(n.name);
    }

    public void visit(QualifiedNameExpr n, Object arg)
    throws Exception
    {
        n.qualifier.accept(this, arg);
        printer.print(".");
        printer.print(n.name);
    }

    public void visit(ImportDeclaration n, Object arg)
    throws Exception
    {
        printer.print("import ");
        if (n.isStatic) {
            printer.print("static ");
        }
        n.name.accept(this, arg);
        if (n.isAsterisk) {
            printer.print(".*");
        }
        printer.println(";");
    }

    public void visit(ClassOrInterfaceDeclaration n, Object arg)
    throws Exception
    {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        if (n.isInterface) {
            printer.print("interface ");
        } else {
            printer.print("class ");
        }

        visitClassBody (n, arg);
	}

	private void visitClassBody (ClassOrInterfaceDeclaration n, Object arg)
    throws Exception
	{
        printer.print(n.name);

        if (n.typeParameters != null) {
            printer.print("<");
            for (Iterator<TypeParameter> i = n.typeParameters.iterator(); i.hasNext();) {
                TypeParameter t = i.next();
                t.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print(">");
        }

        if (n.extend != null) {
            printer.print(" extends ");
            n.extend.accept(this, arg);
        }

        if (n.implementsList != null) {
            printer.print(" implements ");
            for (Iterator<ClassOrInterfaceType> i = n.implementsList.iterator(); i.hasNext();) {
                ClassOrInterfaceType c = i.next();
                c.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }

        printer.println(" {");
        printer.indent();
        if (n.members != null) {
            printMembers(n.members, arg);
        }
        printer.unindent();
        printer.print("}");
    }

    public void visit(EmptyTypeDeclaration n, Object arg)
    throws Exception
    {
        printer.print(";");
    }

    public void visit(ClassOrInterfaceType n, Object arg)
    throws Exception
    {
        if (n.scope != null) {
            n.scope.accept(this, arg);
            printer.print(".");
        }
        printer.print(n.name);
        if (n.typeArgs != null) {
            printer.print("<");
            for (Iterator<Type> i = n.typeArgs.iterator(); i.hasNext();) {
                Type t = i.next();
                t.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print(">");
        }

    }

    public void visit(TypeParameter n, Object arg)
    throws Exception
    {
        printer.print(n.name);
        if (n.typeBound != null) {
            printer.print(" extends ");
            for (Iterator<ClassOrInterfaceType> i = n.typeBound.iterator(); i.hasNext();) {
                ClassOrInterfaceType c = i.next();
                c.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(" & ");
                }
            }
        }
    }

    public void visit(PrimitiveType n, Object arg)
    throws Exception
    {
        switch (n.type) {
            case Boolean:
                printer.print("boolean");
                break;
            case Byte:
                printer.print("byte");
                break;
            case Char:
                printer.print("char");
                break;
            case Double:
                printer.print("double");
                break;
            case Float:
                printer.print("float");
                break;
            case Int:
                printer.print("int");
                break;
            case Long:
                printer.print("long");
                break;
            case Short:
                printer.print("short");
                break;
        }
    }

    public void visit(ReferenceType n, Object arg)
    throws Exception
    {
        n.type.accept(this, arg);
        for (int i = 0; i < n.arrayCount; i++) {
            printer.print("[]");
        }
    }

    public void visit(WildcardType n, Object arg)
    throws Exception
    {
        printer.print("?");
        if (n.ext != null) {
            printer.print(" extends ");
            n.ext.accept(this, arg);
        }
        if (n.sup != null) {
            printer.print(" super ");
            n.sup.accept(this, arg);
        }
    }

    public void visit(FieldDeclaration n, Object arg)
    throws Exception
    {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);
        n.type.accept(this, arg);

        printer.print(" ");
        for (Iterator<VariableDeclarator> i = n.variables.iterator(); i.hasNext();) {
            VariableDeclarator var = i.next();
            var.accept(this, arg);
            if (i.hasNext()) {
                printer.print(", ");
            }
        }

        printer.print(";");
    }

    public void visit(VariableDeclarator n, Object arg)
    throws Exception
    {
        n.id.accept(this, arg);
        if (n.init != null) {
            printer.print(" = ");
            n.init.accept(this, arg);
        }
    }

    public void visit(VariableDeclaratorId n, Object arg)
    throws Exception
    {
        printer.print(n.name);
        for (int i = 0; i < n.arrayCount; i++) {
            printer.print("[]");
        }
    }

    public void visit(ArrayInitializerExpr n, Object arg)
    throws Exception
    {
        printer.print("{");
        if (n.values != null) {
            printer.print(" ");
            for (Iterator<Expression> i = n.values.iterator(); i.hasNext();) {
                Expression expr = i.next();
                expr.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print(" ");
        }
        printer.print("}");
    }

    public void visit(VoidType n, Object arg)
    throws Exception
    {
        printer.print("void");
    }

    public void visit(ArrayAccessExpr n, Object arg)
    throws Exception
    {
        n.name.accept(this, arg);
        printer.print("[");
        n.index.accept(this, arg);
        printer.print("]");
    }

    public void visit(ArrayCreationExpr n, Object arg)
    throws Exception
    {
        printer.print("new ");
        n.type.accept(this, arg);
        if (n.typeArgs != null) {
            printer.print("<");
            for (Iterator<Type> i = n.typeArgs.iterator(); i.hasNext();) {
                Type type = i.next();
                type.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print(">");
        }
        if (n.dimensions != null) {
            for (Expression dim : n.dimensions) {
                printer.print("[");
                dim.accept(this, arg);
                printer.print("]");
            }
            for (int i = 0; i < n.arrayCount; i++) {
                printer.print("[]");
            }
        } else {
            for (int i = 0; i < n.arrayCount; i++) {
                printer.print("[]");
            }
            printer.print(" ");
            n.initializer.accept(this, arg);
        }
    }

    public void visit(AssignExpr n, Object arg) throws Exception {
        n.target.accept(this, arg);
        printer.print(" ");
        switch (n.op) {
            case assign:
                printer.print("=");
                break;
            case and:
                printer.print("&=");
                break;
            case or:
                printer.print("|=");
                break;
            case xor:
                printer.print("^=");
                break;
            case plus:
                printer.print("+=");
                break;
            case minus:
                printer.print("-=");
                break;
            case rem:
                printer.print("%=");
                break;
            case slash:
                printer.print("/=");
                break;
            case star:
                printer.print("*=");
                break;
            case lShift:
                printer.print("<<=");
                break;
            case rSignedShift:
                printer.print(">>=");
                break;
            case rUnsignedShift:
                printer.print(">>>=");
                break;
        }
        printer.print(" ");
        n.value.accept(this, arg);
    }

    public void visit(BinaryExpr n, Object arg) throws Exception {
        n.left.accept(this, arg);
        printer.print(" ");
        switch (n.op) {
            case or:
                printer.print("||");
                break;
            case and:
                printer.print("&&");
                break;
            case binOr:
                printer.print("|");
                break;
            case binAnd:
                printer.print("&");
                break;
            case xor:
                printer.print("^");
                break;
            case equals:
                printer.print("==");
                break;
            case notEquals:
                printer.print("!=");
                break;
            case less:
                printer.print("<");
                break;
            case greater:
                printer.print(">");
                break;
            case lessEquals:
                printer.print("<=");
                break;
            case greaterEquals:
                printer.print(">=");
                break;
            case lShift:
                printer.print("<<");
                break;
            case rSignedShift:
                printer.print(">>");
                break;
            case rUnsignedShift:
                printer.print(">>>");
                break;
            case plus:
                printer.print("+");
                break;
            case minus:
                printer.print("-");
                break;
            case times:
                printer.print("*");
                break;
            case divide:
                printer.print("/");
                break;
            case remainder:
                printer.print("%");
                break;
        }
        printer.print(" ");
        n.right.accept(this, arg);
    }

    public void visit(CastExpr n, Object arg) throws Exception {
        printer.print("((");
        n.type.accept(this, arg);
        printer.print(") ");
        n.expr.accept(this, arg);
        printer.print(")");
    }

    public void visit(ClassExpr n, Object arg) throws Exception {
        n.type.accept(this, arg);
        printer.print(".class");
    }

    public void visit(ConditionalExpr n, Object arg) throws Exception {
        n.condition.accept(this, arg);
        printer.print(" ? ");
        n.thenExpr.accept(this, arg);
        printer.print(" : ");
        n.elseExpr.accept(this, arg);
    }

    public void visit(EnclosedExpr n, Object arg) throws Exception {
        printer.print("(");
        n.inner.accept(this, arg);
        printer.print(")");
    }

    public void visit(FieldAccessExpr n, Object arg) throws Exception {
        n.scope.accept(this, arg);
        printer.print(".");
        printer.print(n.field);
    }

    public void visit(InstanceOfExpr n, Object arg) throws Exception {
        n.expr.accept(this, arg);
        printer.print(" instanceof ");
        n.type.accept(this, arg);
    }

    public void visit(CharLiteralExpr n, Object arg) throws Exception {
        printer.print("'");
        printer.print(n.value);
        printer.print("'");
    }

    public void visit(DoubleLiteralExpr n, Object arg) throws Exception {
        printer.print(n.value);
    }

    public void visit(IntegerLiteralExpr n, Object arg) throws Exception {
        printer.print(n.value);
    }

    public void visit(LongLiteralExpr n, Object arg) throws Exception {
        printer.print(n.value);
    }

    public void visit(IntegerLiteralMinValueExpr n, Object arg) throws Exception {
        printer.print(n.value);
    }

    public void visit(LongLiteralMinValueExpr n, Object arg) throws Exception {
        printer.print(n.value);
    }

    public void visit(StringLiteralExpr n, Object arg) throws Exception {
        printer.print("\"");
        printer.print(n.value);
        printer.print("\"");
    }

    public void visit(BooleanLiteralExpr n, Object arg) throws Exception {
        printer.print(n.value.toString());
    }

    public void visit(NullLiteralExpr n, Object arg) throws Exception {
        printer.print("null");
    }

    public void visit(ThisExpr n, Object arg) throws Exception {
        if (n.classExpr != null) {
            n.classExpr.accept(this, arg);
            printer.print(".");
        }
        printer.print("this");
    }

    public void visit(SuperExpr n, Object arg) throws Exception {
        if (n.classExpr != null) {
            n.classExpr.accept(this, arg);
            printer.print(".");
        }
        printer.print("super");
    }

    public void visit(MethodCallExpr n, Object arg) throws Exception {
        if (n.scope != null) {
            n.scope.accept(this, arg);
            printer.print(".");
        }
        if (n.typeArgs != null) {
        	printer.print ("<");
            for (Iterator<Type> i = n.typeArgs.iterator(); i.hasNext();) {
                Type e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        	printer.print (">");
        }
        printer.print(n.name);
        printer.print("(");
        if (n.args != null) {
            for (Iterator<Expression> i = n.args.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(")");
    }

    public void visit(ObjectCreationExpr n, Object arg)
    throws Exception
    {
    	dumpComment (n, true);
        if (n.scope != null) {
            n.scope.accept(this, arg);
            printer.print(".");
        }

        printer.print("new ");

        n.type.accept(this, arg);

        if (n.typeArgs != null) {
            for (Iterator<Type> i = n.typeArgs.iterator(); i.hasNext();) {
                Type t = i.next();
                t.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }

        printer.print("(");
        if (n.args != null) {
            for (Iterator<Expression> i = n.args.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(")");

        if (n.anonymousClassBody != null) {
            printer.println(" {");
            printer.indent();
            printMembers(n.anonymousClassBody, arg);
            printer.unindent();
            printer.print("}");
        }
    	dumpComment (n, false);
    }

    public void visit(SuperMemberAccessExpr n, Object arg) throws Exception {
        printer.print("super.");
        printer.print(n.name);
    }

    public void visit(UnaryExpr n, Object arg) throws Exception {
        switch (n.op) {
            case positive:
                printer.print("+");
                break;
            case negative:
                printer.print("-");
                break;
            case inverse:
                printer.print("~");
                break;
            case not:
                printer.print("!");
                break;
            case preIncrement:
                printer.print("++");
                break;
            case preDecrement:
                printer.print("--");
                break;
        }

        n.expr.accept(this, arg);

        switch (n.op) {
            case posIncrement:
                printer.print("++");
                break;
            case posDecrement:
                printer.print("--");
                break;
        }
    }

    public void visit(ConstructorDeclaration n, Object arg) throws Exception {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        if (n.typeParameters != null) {
            printer.print("<");
            for (Iterator<TypeParameter> i = n.typeParameters.iterator(); i.hasNext();) {
                TypeParameter t = i.next();
                t.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print("> ");
        }

        printer.print(n.name);

        printer.print("(");
        if (n.parameters != null) {
            for (Iterator<Parameter> i = n.parameters.iterator(); i.hasNext();) {
                Parameter p = i.next();
                p.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(")");

        if (n.throws_ != null) {
            printer.print(" throws ");
            for (Iterator<NameExpr> i = n.throws_.iterator(); i.hasNext();) {
                NameExpr name = i.next();
                name.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(" ");
        n.block.accept(this, arg);
    }

    public void visit(MethodDeclaration n, Object arg) throws Exception {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        if (n.typeParameters != null) {
            printer.print("<");
            for (Iterator<TypeParameter> i = n.typeParameters.iterator(); i.hasNext();) {
                TypeParameter t = i.next();
                t.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print("> ");
        }

        n.type.accept(this, arg);
        printer.print(" ");
        printer.print(n.name);

        printer.print("(");
        if (n.parameters != null) {
            for (Iterator<Parameter> i = n.parameters.iterator(); i.hasNext();) {
                Parameter p = i.next();
                p.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(")");

        for (int i = 0; i < n.arrayCount; i++) {
            printer.print("[]");
        }

        if (n.throws_ != null) {
            printer.print(" throws ");
            for (Iterator<NameExpr> i = n.throws_.iterator(); i.hasNext();) {
                NameExpr name = i.next();
                name.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        if (n.block == null) {
            printer.print(";");
        } else {
            printer.print(" ");
            n.block.accept(this, arg);
        }
    }

    public void visit(Parameter n, Object arg) throws Exception {
        printAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        n.type.accept(this, arg);
        if (n.isVarArgs) {
            printer.print("...");
        }
        printer.print(" ");
        n.id.accept(this, arg);
    }

    public void visit(ExplicitConstructorInvocationStmt n, Object arg) throws Exception {
        if (n.isThis) {
            printer.print("this");
        } else {
            if (n.expr != null) {
                n.expr.accept(this, arg);
                printer.print(".");
            }
            printer.print("super");
        }
        printer.print("(");
        if (n.args != null) {
            for (Iterator<Expression> i = n.args.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(");");
    }

    public void visit(VariableDeclarationExpr n, Object arg) throws Exception {
        printAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        n.type.accept(this, arg);
        printer.print(" ");

        for (Iterator<VariableDeclarator> i = n.vars.iterator(); i.hasNext();) {
            VariableDeclarator v = i.next();
            v.accept(this, arg);
            if (i.hasNext()) {
                printer.print(", ");
            }
        }
    }

    public void visit(TypeDeclarationStmt n, Object arg) throws Exception {
        n.typeDecl.accept(this, arg);
    }

    public void visit(AssertStmt n, Object arg) throws Exception {
        printer.print("assert ");
        n.check.accept(this, arg);
        if (n.msg != null) {
            printer.print(" : ");
            n.msg.accept(this, arg);
        }
        printer.print(";");
    }

    public void visit (BlockStmt n, Object arg)
    throws Exception
    {
    	dumpComment (n, true);
        printer.println("{");
        if (n.stmts != null) {
            printer.indent();
            for (Statement s : n.stmts) {
                s.accept(this, arg);
                printer.println();
            }
            printer.unindent();
        }
        printer.println("}");
    	dumpComment (n, false);
    }

    public void visit(LabeledStmt n, Object arg) throws Exception {
        printer.print(n.label);
        printer.print(": ");
        n.stmt.accept(this, arg);
    }

    public void visit(EmptyStmt n, Object arg) throws Exception {
        printer.print(";");
    }

    public void visit(ExpressionStmt n, Object arg) throws Exception {
        n.expr.accept(this, arg);
        printer.print(";");
    }

    public void visit(SwitchStmt n, Object arg) throws Exception {
        printer.print("switch(");
        n.selector.accept(this, arg);
        printer.println(") {");
        if (n.entries != null) {
            printer.indent();
            for (SwitchEntryStmt e : n.entries) {
                e.accept(this, arg);
            }
            printer.unindent();
        }
        printer.print("}");

    }

    public void visit(SwitchEntryStmt n, Object arg) throws Exception {
        if (n.label != null) {
            printer.print("case ");
            n.label.accept(this, arg);
            printer.print(":");
        } else {
            printer.print("default:");
        }
        printer.println();
        printer.indent();
        if (n.stmts != null) {
            for (Statement s : n.stmts) {
                s.accept(this, arg);
                printer.println();
            }
        }
        printer.unindent();
    }

    public void visit(BreakStmt n, Object arg) throws Exception {
        printer.print("break");
        if (n.id != null) {
            printer.print(" ");
            printer.print(n.id);
        }
        printer.print(";");
    }

    public void visit(ReturnStmt n, Object arg) throws Exception {
        printer.print("return");
        if (n.expr != null) {
            printer.print(" ");
            n.expr.accept(this, arg);
        }
        printer.print(";");
    }

    public void visit(EnumDeclaration n, Object arg) throws Exception {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        printer.print("enum ");
        printer.print(n.name);

        if (n.implementsList != null) {
            printer.print(" implements ");
            for (Iterator<ClassOrInterfaceType> i = n.implementsList.iterator(); i.hasNext();) {
                ClassOrInterfaceType c = i.next();
                c.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }

        printer.println(" {");
        printer.indent();
        if (n.entries != null) {
            printer.println();
            for (Iterator<EnumConstantDeclaration> i = n.entries.iterator(); i.hasNext();) {
                EnumConstantDeclaration e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        if (n.members != null) {
            printer.println(";");
            printMembers(n.members, arg);
        } else {
            printer.println();
        }
        printer.unindent();
        printer.print("}");
    }

    public void visit(EnumConstantDeclaration n, Object arg) throws Exception {
        printer.print(n.name);
        if (n.args != null) {
            printer.print("(");
            for (Iterator<Expression> i = n.args.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
            printer.print(")");
        }

        if (n.classBody != null) {
            printer.println(" {");
            printer.indent();
            printMembers(n.classBody, arg);
            printer.unindent();
            printer.println("}");
        }
    }

    public void visit(EmptyMemberDeclaration n, Object arg) throws Exception {
        printer.print(";");
    }

    public void visit(InitializerDeclaration n, Object arg) throws Exception {
        printer.print("static ");
        n.block.accept(this, arg);
    }

    public void visit(TrishulTaintDeclaration n, Object arg) throws Exception {
        printer.print ("trishultaint ");
        visitClassBody (n, arg);
    }

    public void visit (EngineTaintDeclaration n, Object arg) throws Exception {
        printer.print ("enginetaint ");
        printer.print (n.name);
        if (n.extend != null)
        {
        	printer.print ("\nextend ");
        	printer.print (n.extend.name);
        }

        printer.print ("\n{\n");
        boolean first = true;
        for (EngineTaintConstantDeclaration child : n.entries)
        {
        	if (first) first = false;
        	else printer.print (",\n");
        	child.accept (this, arg);
        }
        printer.print ("\n}\n");
    }

    public void visit(EngineTaintConstantDeclaration n, Object arg) throws Exception {
        printer.print (n.name);
    }


    public void visit(IfStmt n, Object arg) throws Exception {
        printer.print("if (");
        n.condition.accept(this, arg);
        printer.print(") ");
        n.thenStmt.accept(this, arg);
        if (n.elseStmt != null) {
            printer.print(" else ");
            n.elseStmt.accept(this, arg);
        }
    }

    public void visit(WhileStmt n, Object arg) throws Exception {
        printer.print("while (");
        n.condition.accept(this, arg);
        printer.print(") ");
        n.body.accept(this, arg);
    }

    public void visit(ContinueStmt n, Object arg) throws Exception {
        printer.print("continue");
        if (n.id != null) {
            printer.print(" ");
            printer.print(n.id);
        }
        printer.print(";");
    }

    public void visit(DoStmt n, Object arg) throws Exception {
        printer.print("do ");
        n.body.accept(this, arg);
        printer.print(" while (");
        n.condition.accept(this, arg);
        printer.print(");");
    }

    public void visit(ForeachStmt n, Object arg) throws Exception {
        printer.print("for (");
        n.var.accept(this, arg);
        printer.print(" : ");
        n.iterable.accept(this, arg);
        printer.print(") ");
        n.body.accept(this, arg);
    }

    public void visit(ForStmt n, Object arg) throws Exception {
        printer.print("for (");
        if (n.init != null) {
            for (Iterator<Expression> i = n.init.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print("; ");
        n.iterable.accept(this, arg);
        printer.print("; ");
        if (n.update != null) {
            for (Iterator<Expression> i = n.update.iterator(); i.hasNext();) {
                Expression e = i.next();
                e.accept(this, arg);
                if (i.hasNext()) {
                    printer.print(", ");
                }
            }
        }
        printer.print(") ");
        n.body.accept(this, arg);
    }

    public void visit(ThrowStmt n, Object arg) throws Exception {
        printer.print("throw ");
        n.expr.accept(this, arg);
        printer.print(";");
    }

    public void visit(SynchronizedStmt n, Object arg) throws Exception {
        printer.print("synchronized (");
        n.expr.accept(this, arg);
        printer.print(") ");
        n.block.accept(this, arg);
    }

    public void visit(TryStmt n, Object arg) throws Exception {
        printer.print("try ");
        n.tryBlock.accept(this, arg);
        if (n.catchs != null) {
            for (CatchClause c : n.catchs) {
                c.accept(this, arg);
            }
        }
        if (n.finallyBlock != null) {
            printer.print(" finally ");
            n.finallyBlock.accept(this, arg);
        }
    }

    public void visit(CatchClause n, Object arg) throws Exception {
        printer.print(" catch (");
        n.except.accept(this, arg);
        printer.print(") ");
        n.catchBlock.accept(this, arg);

    }

    public void visit(AnnotationDeclaration n, Object arg) throws Exception {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        printer.print("@interface ");
        printer.print(n.name);
        printer.println(" {");
        printer.indent();
        if (n.members != null) {
            printMembers(n.members, arg);
        }
        printer.unindent();
        printer.print("}");
    }

    public void visit(AnnotationMemberDeclaration n, Object arg) throws Exception {
        printMemberAnnotations(n.annotations, arg);
        printModifiers(n.modifiers);

        n.type.accept(this, arg);
        printer.print(" ");
        printer.print(n.name);
        printer.print("()");
        if (n.defaultValue != null) {
            printer.print(" default ");
            n.defaultValue.accept(this, arg);
        }
        printer.print(";");
    }

    public void visit(MarkerAnnotationExpr n, Object arg) throws Exception {
        printer.print("@");
        n.name.accept(this, arg);
    }

    public void visit(SingleMemberAnnotationExpr n, Object arg) throws Exception {
        printer.print("@");
        n.name.accept(this, arg);
        printer.print("(");
        n.memberValue.accept(this, arg);
        printer.print(")");
    }

    public void visit(NormalAnnotationExpr n, Object arg) throws Exception {
        printer.print("@");
        n.name.accept(this, arg);
        printer.print("(");
        for (Iterator<MemberValuePair> i = n.pairs.iterator(); i.hasNext();) {
            MemberValuePair m = i.next();
            m.accept(this, arg);
            if (i.hasNext()) {
                printer.print(", ");
            }
        }
        printer.print(")");
    }

    public void visit(MemberValuePair n, Object arg) throws Exception {
        printer.print(n.name);
        printer.print(" = ");
        n.value.accept(this, arg);
    }


    public void visit(PolymerActionSwitchStmt n, Object arg) throws Exception {
        printer.print("aswitch(");
        n.selector.accept(this, arg);
        printer.println(") {");
        if (n.entries != null) {
            printer.indent();
            for (PolymerActionSwitchEntryStmt e : n.entries) {
                e.accept(this, arg);
            }
            printer.unindent();
        }
        printer.print("}");

    }

    public void visit(PolymerActionSwitchEntryStmt n, Object arg) throws Exception {
        if (n.label != null) {
            printer.print("case ");
            n.label.accept(this, arg);
            printer.print(":");
        } else {
            printer.print("default:");
        }
        printer.println();
        printer.indent();
        if (n.stmts != null) {
            for (Statement s : n.stmts) {
                s.accept(this, arg);
                printer.println();
            }
        }
        printer.unindent();
    }

    public void visit(ActionPatternExpr n, Object arg) throws Exception {
        printer.print(n.pattern.toString ());
    }

    public void visit(TaintPatternExpr n, Object arg)
    throws Exception
    {
        printer.print ("new trishul_p.TaintPattern (");
        if (n.taintPattern.hasTaint ())
        	printTaint (n.taintPattern.getTaint ());
        else
        	printer.print (n.taintPattern.getTaintName ());
        printer.print (", ");
        printer.print (n.taintPattern.matchAll () ? "true" : "false");
        printer.print (", ");
        printer.print (n.taintPattern.hasSaveName () ? n.taintPattern.getSaveName () : "null");
        printer.print (")");
    }

    public void visit (TaintExpr n, Object arg)
    throws Exception
    {
    	printTaint (n.taint);
    }

	private void printTaint (Taint t)
	{
		String s = null;
		if (t instanceof NamedTaint)
		{
			for (String name: ((NamedTaint) t).taintNames ())
			{
				if (s == null) s = name;
				else s = s + " | " + name;
			}
			s = "(" + s + ")";
		}
		else
		{
        	s = "" + t.getTaint ();
        }
       	printer.print ("new trishul_p.Taint (" + t.getType () + ", " + s + ")");
	}

    private void dumpComment (Node n, boolean before)
    {
    	if (n.hasComment (before))
    	{
    		printer.println (n.getComment (before));
    	}
    }

    public void visit (TaintLiteralExpr n, Object arg)
    throws Exception
    {
        printer.print ("#{");

        boolean first = true;
        for (String s: n.values)
        {
        	if (first) first = false;
        	else printer.print (", ");
        	printer.print (s);
        }

        printer.print ("}");
	}


}
