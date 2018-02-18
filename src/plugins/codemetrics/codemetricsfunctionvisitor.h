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

#pragma once

#include "codemetricsitem.h"

#include <cplusplus/ASTVisitor.h>

#include <QStack>
#include <QVector>

namespace CodeMetrics {
namespace Internal {

// This visitor collects code metrics of functions. It computes cyclomatic
// complexity, number of instructions and lines of code for each function
// in the source tree.
class CodeMetricsFunctionVisitor : public CPlusPlus::ASTVisitor
{
public:
    CodeMetricsFunctionVisitor(CPlusPlus::TranslationUnit *unit);
    virtual ~CodeMetricsFunctionVisitor();

    QVector<CodeMetricsFunctionItem> takeItems();

    // ASTVisitor interface
public:
    virtual bool visit(CPlusPlus::AccessDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::AliasDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::AlignmentSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::AlignofExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::AnonymousNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ArrayAccessAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ArrayDeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ArrayInitializerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::AsmDefinitionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BaseSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BinaryExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BoolLiteralAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BracedInitializerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BracketDesignatorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::BreakStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CallAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CaptureAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CaseStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CastExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CatchClauseAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ClassSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CompoundExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CompoundLiteralAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CompoundStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ConditionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ConditionalExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ContinueStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ConversionFunctionIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CppCastExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::CtorInitializerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DeclarationStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DeclaratorIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DecltypeSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DeleteExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DesignatedInitializerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DestructorNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DoStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DotDesignatorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::DynamicExceptionSpecificationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ElaboratedTypeSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::EmptyDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::EnumSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::EnumeratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ExceptionDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ExpressionListParenAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ExpressionOrDeclarationStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ExpressionStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ForStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ForeachStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::FunctionDeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::FunctionDefinitionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::GnuAttributeAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::GnuAttributeSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::GotoStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::IdExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::IfStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LabeledStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LambdaCaptureAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LambdaDeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LambdaExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LambdaIntroducerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LinkageBodyAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::LinkageSpecificationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::MemInitializerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::MemberAccessAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NamedTypeSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NamespaceAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NamespaceAliasDefinitionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NestedDeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NestedExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NestedNameSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NewArrayDeclaratorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NewExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NewTypeIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NoExceptOperatorExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NoExceptSpecificationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::NumericLiteralAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCClassDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCClassForwardDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCDynamicPropertiesDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCEncodeExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCFastEnumerationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCInstanceVariablesDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCMessageArgumentAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCMessageArgumentDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCMessageExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCMethodDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCMethodPrototypeAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCPropertyAttributeAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCPropertyDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCProtocolDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCProtocolExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCProtocolForwardDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCProtocolRefsAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSelectorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSelectorArgumentAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSelectorExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSynchronizedStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSynthesizedPropertiesDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCSynthesizedPropertyAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCTypeNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ObjCVisibilityDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::OperatorAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::OperatorFunctionIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ParameterDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ParameterDeclarationClauseAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::PointerAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::PointerLiteralAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::PointerToMemberAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::PostIncrDecrAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtEnumDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtFlagsDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtInterfaceNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtInterfacesDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtMemberDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtMethodAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtObjectTagAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtPrivateSlotAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtPropertyDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QtPropertyDeclarationItemAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::QualifiedNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::RangeBasedForStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ReferenceAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ReturnStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::SimpleDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::SimpleNameAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::SimpleSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::SizeofExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::StaticAssertDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::StringLiteralAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::SwitchStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TemplateDeclarationAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TemplateIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TemplateTypeParameterAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ThisExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::ThrowExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TrailingReturnTypeAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TranslationUnitAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TryBlockStatementAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypeConstructorCallAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypeIdAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypeidExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypenameCallExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypenameTypeParameterAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::TypeofSpecifierAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::UnaryExpressionAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::UsingAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::UsingDirectiveAST *) Q_DECL_OVERRIDE;
    virtual bool visit(CPlusPlus::WhileStatementAST *) Q_DECL_OVERRIDE;
    virtual void endVisit(CPlusPlus::FunctionDefinitionAST *) Q_DECL_OVERRIDE;
    virtual void endVisit(CPlusPlus::ClassSpecifierAST *) Q_DECL_OVERRIDE;
    virtual void endVisit(CPlusPlus::TranslationUnitAST *) Q_DECL_OVERRIDE;

private:
    void incCyclomaticComplexity(int amount = 1);
    void incInstructions(int amount = 1);

    QStack<CodeMetricsFunctionItem> m_functionStack;
    QVector<CodeMetricsFunctionItem> m_collectedItems;
};

} // namespace Internal
} // namespace CodeMetrics
