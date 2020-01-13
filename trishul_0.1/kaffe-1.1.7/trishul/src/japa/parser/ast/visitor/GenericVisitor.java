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
public interface GenericVisitor<R, A> {

    public R visit(Node n, A arg) throws Exception;

    //- Compilation Unit ----------------------------------

    public R visit(CompilationUnit n, A arg) throws Exception;

    public R visit(ImportDeclaration n, A arg) throws Exception;

    public R visit(TypeParameter n, A arg) throws Exception;

    //- Body ----------------------------------------------

    public R visit(ClassOrInterfaceDeclaration n, A arg) throws Exception;

    public R visit(EnumDeclaration n, A arg) throws Exception;

    public R visit(EmptyTypeDeclaration n, A arg) throws Exception;

    public R visit(EnumConstantDeclaration n, A arg) throws Exception;

    public R visit(AnnotationDeclaration n, A arg) throws Exception;

    public R visit(AnnotationMemberDeclaration n, A arg) throws Exception;

    public R visit(FieldDeclaration n, A arg) throws Exception;

    public R visit(VariableDeclarator n, A arg) throws Exception;

    public R visit(VariableDeclaratorId n, A arg) throws Exception;

    public R visit(ConstructorDeclaration n, A arg) throws Exception;

    public R visit(MethodDeclaration n, A arg) throws Exception;

    public R visit(Parameter n, A arg) throws Exception;

    public R visit(EmptyMemberDeclaration n, A arg) throws Exception;

    public R visit(InitializerDeclaration n, A arg) throws Exception;

    public R visit(TrishulTaintDeclaration n, A arg) throws Exception;

    public R visit(EngineTaintDeclaration n, A arg) throws Exception;

    public R visit(EngineTaintConstantDeclaration n, A arg) throws Exception;

    //- Type ----------------------------------------------

    public R visit(ClassOrInterfaceType n, A arg) throws Exception;

    public R visit(PrimitiveType n, A arg) throws Exception;

    public R visit(ReferenceType n, A arg) throws Exception;

    public R visit(VoidType n, A arg) throws Exception;

    public R visit(WildcardType n, A arg) throws Exception;

    //- Expression ----------------------------------------

    public R visit(ArrayAccessExpr n, A arg) throws Exception;

    public R visit(ArrayCreationExpr n, A arg) throws Exception;

    public R visit(ArrayInitializerExpr n, A arg) throws Exception;

    public R visit(AssignExpr n, A arg) throws Exception;

    public R visit(BinaryExpr n, A arg) throws Exception;

    public R visit(CastExpr n, A arg) throws Exception;

    public R visit(ClassExpr n, A arg) throws Exception;

    public R visit(ConditionalExpr n, A arg) throws Exception;

    public R visit(EnclosedExpr n, A arg) throws Exception;

    public R visit(FieldAccessExpr n, A arg) throws Exception;

    public R visit(InstanceOfExpr n, A arg) throws Exception;

    public R visit(StringLiteralExpr n, A arg) throws Exception;

    public R visit(IntegerLiteralExpr n, A arg) throws Exception;

    public R visit(LongLiteralExpr n, A arg) throws Exception;

    public R visit(IntegerLiteralMinValueExpr n, A arg) throws Exception;

    public R visit(LongLiteralMinValueExpr n, A arg) throws Exception;

    public R visit(CharLiteralExpr n, A arg) throws Exception;

    public R visit(DoubleLiteralExpr n, A arg) throws Exception;

    public R visit(BooleanLiteralExpr n, A arg) throws Exception;

    public R visit(NullLiteralExpr n, A arg) throws Exception;

    public R visit(MethodCallExpr n, A arg) throws Exception;

    public R visit(NameExpr n, A arg) throws Exception;

    public R visit(ObjectCreationExpr n, A arg) throws Exception;

    public R visit(QualifiedNameExpr n, A arg) throws Exception;

    public R visit(SuperMemberAccessExpr n, A arg) throws Exception;

    public R visit(ThisExpr n, A arg) throws Exception;

    public R visit(SuperExpr n, A arg) throws Exception;

    public R visit(UnaryExpr n, A arg) throws Exception;

    public R visit(VariableDeclarationExpr n, A arg) throws Exception;

    public R visit(MarkerAnnotationExpr n, A arg) throws Exception;

    public R visit(SingleMemberAnnotationExpr n, A arg) throws Exception;

    public R visit(NormalAnnotationExpr n, A arg) throws Exception;

    public R visit(MemberValuePair n, A arg) throws Exception;

    public R visit(ActionPatternExpr n, A arg) throws Exception;

    public R visit(TaintPatternExpr n, A arg) throws Exception;
    public R visit(TaintExpr n, A arg) throws Exception;
    public R visit(TaintLiteralExpr n, A arg) throws Exception;

    //- Statements ----------------------------------------

    public R visit(ExplicitConstructorInvocationStmt n, A arg) throws Exception;

    public R visit(TypeDeclarationStmt n, A arg) throws Exception;

    public R visit(AssertStmt n, A arg) throws Exception;

    public R visit(BlockStmt n, A arg) throws Exception;

    public R visit(LabeledStmt n, A arg) throws Exception;

    public R visit(EmptyStmt n, A arg) throws Exception;

    public R visit(ExpressionStmt n, A arg) throws Exception;

    public R visit(SwitchStmt n, A arg) throws Exception;

    public R visit(SwitchEntryStmt n, A arg) throws Exception;

    public R visit(BreakStmt n, A arg) throws Exception;

    public R visit(ReturnStmt n, A arg) throws Exception;

    public R visit(IfStmt n, A arg) throws Exception;

    public R visit(WhileStmt n, A arg) throws Exception;

    public R visit(ContinueStmt n, A arg) throws Exception;

    public R visit(DoStmt n, A arg) throws Exception;

    public R visit(ForeachStmt n, A arg) throws Exception;

    public R visit(ForStmt n, A arg) throws Exception;

    public R visit(ThrowStmt n, A arg) throws Exception;

    public R visit(SynchronizedStmt n, A arg) throws Exception;

    public R visit(TryStmt n, A arg) throws Exception;

    public R visit(CatchClause n, A arg) throws Exception;

    public R visit(PolymerActionSwitchStmt n, A arg) throws Exception;

    public R visit(PolymerActionSwitchEntryStmt n, A arg) throws Exception;
}
