/****************************************************************************
**
**  Code Metrics plugin - Calculate code metrics for C++ code
**  Copyright (C) 2018  Jakub Melka
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*****************************************************************************/

#include "codemetricsfunctionvisitor.h"

#include <cplusplus/AST.h>
#include <cplusplus/Token.h>
#include <cplusplus/Overview.h>
#include <cplusplus/Symbols.h>
#include <cplusplus/LookupContext.h>

namespace CodeMetrics {
namespace Internal {

template<typename T>
int calculateListSize(const CPlusPlus::List<T>* list)
{
    int count = 0;
    while (list) {
        ++count;
        list = list->next;
    }
    return count;
}

CodeMetricsFunctionVisitor::CodeMetricsFunctionVisitor(CPlusPlus::TranslationUnit *unit) :
    CPlusPlus::ASTVisitor(unit)
{

}

CodeMetricsFunctionVisitor::~CodeMetricsFunctionVisitor()
{

}

QVector<CodeMetricsFunctionItem> CodeMetricsFunctionVisitor::takeItems()
{
    m_collectedItems.squeeze();
    return qMove(m_collectedItems);
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AccessDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AliasDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AlignmentSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AlignofExpressionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AnonymousNameAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ArrayAccessAST *)
{
    // Access of the array involves at least one instruction. Assume
    // we access a simple type.
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ArrayDeclaratorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ArrayInitializerAST *ast)
{
    int count = calculateListSize(ast->expression_list);
    incInstructions(count);
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::AsmDefinitionAST *)
{
    // We cant analyze assembler code. We assume it is one operation.
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BaseSpecifierAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BinaryExpressionAST *ast)
{
    const CPlusPlus::Token &token = tokenAt(ast->binary_op_token);

    switch (token.kind()) {
    case CPlusPlus::T_OR:
    case CPlusPlus::T_AND:
    case CPlusPlus::T_QUESTION:
        // C++ binary operators || and && can cause conditional jump,
        // because we have partial expression evaluation. Also ternary operator,
        // condition ? expr1 : expr2 causes conditional jump.
        incCyclomaticComplexity();
        break;

    default:
        break;
    }

    // We assume this is a single instruction
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BoolLiteralAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BracedInitializerAST *ast)
{
    int count = calculateListSize(ast->expression_list);
    incInstructions(count);
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BracketDesignatorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::BreakStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CallAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CaptureAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CaseStatementAST *)
{
    // A switch-case statement can be represented as series
    // of if-else statement. So a case statement is equivalent
    // to the if statement.
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CastExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CatchClauseAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ClassSpecifierAST *)
{
    // Ignore nested class in the function. For this reason, we introduce
    // a new context in the current context.
    m_functionStack.push(CodeMetricsFunctionItem());
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CompoundExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CompoundLiteralAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CompoundStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ConditionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ConditionalExpressionAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ContinueStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ConversionFunctionIdAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CppCastExpressionAST *ast)
{
    // We assume cast is simple - 1 instruction.Then we parse only
    // the casted expression.
    incInstructions();
    accept(ast->expression);
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::CtorInitializerAST *ast)
{
    // We assume we must perform same number of operations, as the number
    // of the initialized member variables.
    int count = calculateListSize(ast->member_initializer_list);
    incInstructions(count);
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DeclarationStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DeclaratorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DeclaratorIdAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DecltypeSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DeleteExpressionAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DesignatedInitializerAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DestructorNameAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DoStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DotDesignatorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::DynamicExceptionSpecificationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ElaboratedTypeSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::EmptyDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::EnumSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::EnumeratorAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ExceptionDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ExpressionListParenAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ExpressionOrDeclarationStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ExpressionStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ForStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ForeachStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::FunctionDeclaratorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::FunctionDefinitionAST *ast)
{
    // Here we must introduce a new stack context.
    CodeMetricsFunctionItem item;
    item.init();

    // Get the fully qualified name
    CPlusPlus::Overview overview;
    if (ast->symbol)
        item.qualifiedFunctionName = overview.prettyName(CPlusPlus::LookupContext::fullyQualifiedName(ast->symbol));
    else if (ast->declarator && ast->declarator->core_declarator) {
        // For some reason, we cannot get the symbol from the ast tree. We use raw declarator.
        CPlusPlus::CoreDeclaratorAST *declarator = ast->declarator->core_declarator;
        unsigned firstDeclaratorToken = declarator->firstToken();
        unsigned lastDeclaratorToken = declarator->lastToken();

        for (unsigned tokenIndex = firstDeclaratorToken; tokenIndex <= lastDeclaratorToken; ++tokenIndex) {
            const CPlusPlus::Token& token = tokenAt(tokenIndex);
            const char* tokenSource = translationUnit()->firstSourceChar() + token.bytesBegin();
            item.qualifiedFunctionName += QString::fromUtf8(tokenSource, token.bytes());
        }
    }

    if (ast->function_body) {
        unsigned bodyStartToken = ast->function_body->firstToken();
        unsigned bodyEndToken = ast->function_body->lastToken();
        unsigned startLine = 0;
        unsigned endLine = 0;
        getTokenStartPosition(bodyStartToken, &startLine, nullptr);
        getTokenEndPosition(bodyEndToken, &endLine, nullptr);

        Q_ASSERT_X(endLine >= startLine, __FUNCTION__, "Ending line of the function body is before the start line!");
        item.lines = endLine + 1 - startLine;
    }

    // Get the line number for function definition
    unsigned line = 0;
    unsigned column = 0;
    getTokenStartPosition(ast->firstToken(), &line, &column);
    item.line = line;

    item.linesOfCode = -1;
    item.linesOfComment = -1;

    m_functionStack.push(qMove(item));
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::GnuAttributeAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::GnuAttributeSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::GotoStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::IdExpressionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::IfStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LabeledStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LambdaCaptureAST *ast)
{
    // Capture of the variables involves some instructions to be executed.
    int count = calculateListSize(ast->capture_list);
    incInstructions(count);
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LambdaDeclaratorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LambdaExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LambdaIntroducerAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LinkageBodyAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::LinkageSpecificationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::MemInitializerAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::MemberAccessAST *)
{
    // Accessing a member, or a function involves some instructions
    // to be executed.
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NamedTypeSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NamespaceAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NamespaceAliasDefinitionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NestedDeclaratorAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NestedExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NestedNameSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NewArrayDeclaratorAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NewExpressionAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NewTypeIdAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NoExceptOperatorExpressionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NoExceptSpecificationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::NumericLiteralAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCClassDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCClassForwardDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCDynamicPropertiesDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCEncodeExpressionAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCFastEnumerationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCInstanceVariablesDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCMessageArgumentAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCMessageArgumentDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCMessageExpressionAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCMethodDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCMethodPrototypeAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCPropertyAttributeAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCPropertyDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCProtocolDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCProtocolExpressionAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCProtocolForwardDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCProtocolRefsAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSelectorAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSelectorArgumentAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSelectorExpressionAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSynchronizedStatementAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSynthesizedPropertiesDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCSynthesizedPropertyAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCTypeNameAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ObjCVisibilityDeclarationAST *)
{
    // We do not handle this code
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::OperatorAST *ast)
{
    const CPlusPlus::Token &operatorToken = tokenAt(ast->op_token);

    switch (operatorToken.kind()) {
    case CPlusPlus::T_COMMA:
        break;

    case CPlusPlus::T_NEW:
    case CPlusPlus::T_DELETE:
    case CPlusPlus::T_PLUS:
    case CPlusPlus::T_MINUS:
    case CPlusPlus::T_STAR:
    case CPlusPlus::T_SLASH:
    case CPlusPlus::T_PERCENT:
    case CPlusPlus::T_CARET:
    case CPlusPlus::T_AMPER:
    case CPlusPlus::T_PIPE:
    case CPlusPlus::T_TILDE:
    case CPlusPlus::T_EXCLAIM:
    case CPlusPlus::T_LESS:
    case CPlusPlus::T_GREATER:
    case CPlusPlus::T_AMPER_EQUAL:
    case CPlusPlus::T_CARET_EQUAL:
    case CPlusPlus::T_SLASH_EQUAL:
    case CPlusPlus::T_EQUAL:
    case CPlusPlus::T_EQUAL_EQUAL:
    case CPlusPlus::T_EXCLAIM_EQUAL:
    case CPlusPlus::T_GREATER_EQUAL:
    case CPlusPlus::T_GREATER_GREATER_EQUAL:
    case CPlusPlus::T_LESS_EQUAL:
    case CPlusPlus::T_LESS_LESS_EQUAL:
    case CPlusPlus::T_MINUS_EQUAL:
    case CPlusPlus::T_PERCENT_EQUAL:
    case CPlusPlus::T_PIPE_EQUAL:
    case CPlusPlus::T_PLUS_EQUAL:
    case CPlusPlus::T_STAR_EQUAL:
    case CPlusPlus::T_TILDE_EQUAL:
    case CPlusPlus::T_LESS_LESS:
    case CPlusPlus::T_GREATER_GREATER:
    case CPlusPlus::T_PLUS_PLUS:
    case CPlusPlus::T_MINUS_MINUS:
    case CPlusPlus::T_ARROW_STAR:
    case CPlusPlus::T_DOT_STAR:
    case CPlusPlus::T_ARROW:
        incInstructions();
        break;

    case CPlusPlus::T_PIPE_PIPE:
    case CPlusPlus::T_AMPER_AMPER:
        // These operations involves conditional jump - increase cyclomatic complexity
        incCyclomaticComplexity();
        incInstructions();
        break;

    default:
        break;
    }

    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::OperatorFunctionIdAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ParameterDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ParameterDeclarationClauseAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::PointerAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::PointerLiteralAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::PointerToMemberAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::PostIncrDecrAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtEnumDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtFlagsDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtInterfaceNameAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtInterfacesDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtMemberDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtMethodAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtObjectTagAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtPrivateSlotAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtPropertyDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QtPropertyDeclarationItemAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::QualifiedNameAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::RangeBasedForStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ReferenceAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ReturnStatementAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::SimpleDeclarationAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::SimpleNameAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::SimpleSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::SizeofExpressionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::StaticAssertDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::StringLiteralAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::SwitchStatementAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TemplateDeclarationAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TemplateIdAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TemplateTypeParameterAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ThisExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::ThrowExpressionAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TrailingReturnTypeAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TranslationUnitAST *)
{
    // Global dummy item
    m_functionStack.push(CodeMetricsFunctionItem());
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TryBlockStatementAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypeConstructorCallAST *ast)
{
    accept(ast->expression);
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypeIdAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypeidExpressionAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypenameCallExpressionAST *)
{
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypenameTypeParameterAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::TypeofSpecifierAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::UnaryExpressionAST *)
{
    incInstructions();
    return true;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::UsingAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::UsingDirectiveAST *)
{
    return false;
}

bool CodeMetricsFunctionVisitor::visit(CPlusPlus::WhileStatementAST *)
{
    incCyclomaticComplexity();
    incInstructions();
    return true;
}

void CodeMetricsFunctionVisitor::endVisit(CPlusPlus::FunctionDefinitionAST *)
{
    m_collectedItems.append(m_functionStack.pop());
}

void CodeMetricsFunctionVisitor::endVisit(CPlusPlus::ClassSpecifierAST *)
{
    m_functionStack.pop();
}

void CodeMetricsFunctionVisitor::endVisit(CPlusPlus::TranslationUnitAST *)
{
    m_functionStack.pop();
}

void CodeMetricsFunctionVisitor::incCyclomaticComplexity(int amount)
{
    m_functionStack.top().cyclomaticComplexity += amount;
}

void CodeMetricsFunctionVisitor::incInstructions(int amount)
{
    m_functionStack.top().instructions += amount;
}

} // namespace Internal
} // namespace CodeMetrics
