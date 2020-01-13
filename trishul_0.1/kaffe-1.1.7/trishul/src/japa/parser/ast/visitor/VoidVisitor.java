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

/**
 * @author Julio Vilmar Gesser
 */
public interface VoidVisitor<A> {

    public void visit(Node n, A arg) throws Exception;

    //- Compilation Unit ----------------------------------

    public void visit(CompilationUnit n, A arg) throws Exception;

    public void visit(ImportDeclaration n, A arg) throws Exception;

    public void visit(TypeParameter n, A arg) throws Exception;

    //- Body ----------------------------------------------

    public void visit(ClassOrInterfaceDeclaration n, A arg) throws Exception;

    public void visit(EnumDeclaration n, A arg) throws Exception;

    public void visit(EmptyTypeDeclaration n, A arg) throws Exception;

    public void visit(EnumConstantDeclaration n, A arg) throws Exception;

    public void visit(AnnotationDeclaration n, A arg) throws Exception;

    public void visit(AnnotationMemberDeclaration n, A arg) throws Exception;

    public void visit(FieldDeclaration n, A arg) throws Exception;

    public void visit(VariableDeclarator n, A arg) throws Exception;

    public void visit(VariableDeclaratorId n, A arg) throws Exception;

    public void visit(ConstructorDeclaration n, A arg) throws Exception;

    public void visit(MethodDeclaration n, A arg) throws Exception;

    public void visit(Parameter n, A arg) throws Exception;

    public void visit(EmptyMemberDeclaration n, A arg) throws Exception;

    public void visit(InitializerDeclaration n, A arg) throws Exception;

    public void visit(TrishulTaintDeclaration n, A arg) throws Exception;

    public void visit(EngineTaintDeclaration n, A arg) throws Exception;

    public void visit(EngineTaintConstantDeclaration n, A arg) throws Exception;

    //- Type ----------------------------------------------

    public void visit(ClassOrInterfaceType n, A arg) throws Exception;

    public void visit(PrimitiveType n, A arg) throws Exception;

    public void visit(ReferenceType n, A arg) throws Exception;

    public void visit(VoidType n, A arg) throws Exception;

    public void visit(WildcardType n, A arg) throws Exception;

    //- Expression ----------------------------------------

    public void visit(ArrayAccessExpr n, A arg) throws Exception;

    public void visit(ArrayCreationExpr n, A arg) throws Exception;

    public void visit(ArrayInitializerExpr n, A arg) throws Exception;

    public void visit(AssignExpr n, A arg) throws Exception;

    public void visit(BinaryExpr n, A arg) throws Exception;

    public void visit(CastExpr n, A arg) throws Exception;

    public void visit(ClassExpr n, A arg) throws Exception;

    public void visit(ConditionalExpr n, A arg) throws Exception;

    public void visit(EnclosedExpr n, A arg) throws Exception;

    public void visit(FieldAccessExpr n, A arg) throws Exception;

    public void visit(InstanceOfExpr n, A arg) throws Exception;

    public void visit(StringLiteralExpr n, A arg) throws Exception;

    public void visit(IntegerLiteralExpr n, A arg) throws Exception;

    public void visit(LongLiteralExpr n, A arg) throws Exception;

    public void visit(IntegerLiteralMinValueExpr n, A arg) throws Exception;

    public void visit(LongLiteralMinValueExpr n, A arg) throws Exception;

    public void visit(CharLiteralExpr n, A arg) throws Exception;

    public void visit(DoubleLiteralExpr n, A arg) throws Exception;

    public void visit(BooleanLiteralExpr n, A arg) throws Exception;

    public void visit(NullLiteralExpr n, A arg) throws Exception;

    public void visit(MethodCallExpr n, A arg) throws Exception;

    public void visit(NameExpr n, A arg) throws Exception;

    public void visit(ObjectCreationExpr n, A arg) throws Exception;

    public void visit(QualifiedNameExpr n, A arg) throws Exception;

    public void visit(SuperMemberAccessExpr n, A arg) throws Exception;

    public void visit(ThisExpr n, A arg) throws Exception;

    public void visit(SuperExpr n, A arg) throws Exception;

    public void visit(UnaryExpr n, A arg) throws Exception;

    public void visit(VariableDeclarationExpr n, A arg) throws Exception;

    public void visit(MarkerAnnotationExpr n, A arg) throws Exception;

    public void visit(SingleMemberAnnotationExpr n, A arg) throws Exception;

    public void visit(NormalAnnotationExpr n, A arg) throws Exception;

    public void visit(MemberValuePair n, A arg) throws Exception;

    public void visit (ActionPatternExpr n, A arg) throws Exception;

    public void visit (TaintPatternExpr n, A arg) throws Exception;

    public void visit (TaintExpr n, A arg) throws Exception;

    public void visit (TaintLiteralExpr n, A arg) throws Exception;

    //- Statements ----------------------------------------

    public void visit(ExplicitConstructorInvocationStmt n, A arg) throws Exception;

    public void visit(TypeDeclarationStmt n, A arg) throws Exception;

    public void visit(AssertStmt n, A arg) throws Exception;

    public void visit(BlockStmt n, A arg) throws Exception;

    public void visit(LabeledStmt n, A arg) throws Exception;

    public void visit(EmptyStmt n, A arg) throws Exception;

    public void visit(ExpressionStmt n, A arg) throws Exception;

    public void visit(SwitchStmt n, A arg) throws Exception;

    public void visit(SwitchEntryStmt n, A arg) throws Exception;

    public void visit(BreakStmt n, A arg) throws Exception;

    public void visit(ReturnStmt n, A arg) throws Exception;

    public void visit(IfStmt n, A arg) throws Exception;

    public void visit(WhileStmt n, A arg) throws Exception;

    public void visit(ContinueStmt n, A arg) throws Exception;

    public void visit(DoStmt n, A arg) throws Exception;

    public void visit(ForeachStmt n, A arg) throws Exception;

    public void visit(ForStmt n, A arg) throws Exception;

    public void visit(ThrowStmt n, A arg) throws Exception;

    public void visit(SynchronizedStmt n, A arg) throws Exception;

    public void visit(TryStmt n, A arg) throws Exception;

    public void visit(CatchClause n, A arg) throws Exception;

    public void visit(PolymerActionSwitchStmt n, A arg) throws Exception;

    public void visit(PolymerActionSwitchEntryStmt n, A arg) throws Exception;

}
