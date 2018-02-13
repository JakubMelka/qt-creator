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

#include "codemetricscollector.h"

#include <cplusplus/TranslationUnit.h>

namespace CodeMetrics {
namespace Internal {

CodeMetricsCollector::CodeMetricsCollector(QObject* parent) :
    QObject(parent)
{
    CppTools::CppModelManager *modelManager = CppTools::CppModelManager::instance();

    // Here we really want a direct connection. This slot can be called
    // from another thread, which is here intended.
    connect(modelManager, &CppTools::CppModelManager::documentUpdated,
            this, &CodeMetricsCollector::documentUpdated, Qt::DirectConnection);
}

void CodeMetricsCollector::documentUpdated(CPlusPlus::Document::Ptr document)
{
    CppTools::CppModelManager *cppModelManager = CppTools::CppModelManager::instance();

    if (!cppModelManager->projectPart(document->fileName()).isEmpty())
        collectCodeMetrics(document);
}

void CodeMetricsCollector::collectCodeMetrics(CPlusPlus::Document::Ptr document)
{
    CodeMetricsItemPointer item = CodeMetricsItemPointer::create(Utils::FileName::fromString(document->fileName()));
    CPlusPlus::TranslationUnit *translationUnit = document->translationUnit();

    if (translationUnit->isTokenized()) {
        const unsigned commentCount = translationUnit->commentCount();
        const unsigned tokenCount = translationUnit->tokenCount();

        // Calculate line count
        unsigned lineCount = 0;
        if (commentCount) {
            CPlusPlus::Token lastComment = translationUnit->commentAt(commentCount - 1);
            translationUnit->getPosition(lastComment.utf16charsEnd(), &lineCount);
        }
        if (tokenCount) {
            CPlusPlus::Token lastToken = translationUnit->tokenAt(tokenCount - 1);
            unsigned tokenLineCount = 0;
            translationUnit->getPosition(lastToken.utf16charsEnd(), &tokenLineCount);
            lineCount = qMax(lineCount, tokenLineCount);
        }
        item->lines = static_cast<int>(lineCount);

        // Calculate comment line count
        int commentLineCount = 0;
        for (unsigned i = 0; i < commentCount; ++i) {
            const CPlusPlus::Token &comment = translationUnit->commentAt(i);
            unsigned commentLineStart = 0;
            unsigned commentLineEnd = 0;
            translationUnit->getPosition(comment.utf16charsBegin(), &commentLineStart);
            translationUnit->getPosition(comment.utf16charsEnd(), &commentLineEnd);
            commentLineCount += static_cast<int>(commentLineEnd) - static_cast<int>(commentLineStart) + 1;
        }
        item->linesOfComment = commentLineCount;

        // Calculate code line count
        unsigned lineCodeCount = 0;
        Q_CONSTEXPR unsigned invalidCodeLine = ~0;
        unsigned lastCodeLine = invalidCodeLine;
        for (unsigned i = 0; i < tokenCount; ++i) {
            const CPlusPlus::Token &token = translationUnit->tokenAt(i);

            // Ignore whitespace and comments
            if (token.whitespace() || token.isComment())
                continue;

            unsigned tokenLineStart = 0;
            unsigned tokenLineEnd = 0;
            translationUnit->getPosition(token.utf16charsBegin(), &tokenLineStart);
            translationUnit->getPosition(token.utf16charsEnd(), &tokenLineEnd);

            // FIXME: There is internal bug in the cpp parser, ignore this token!
            // Normally, the token line position should be non-decreasing, but it is
            // not true (for example, end of the token can have lesser line number
            // than start of the token).
            if (tokenLineStart > tokenLineEnd ||
                    (lastCodeLine > tokenLineStart && lastCodeLine != invalidCodeLine))
                continue;

            Q_ASSERT(lastCodeLine <= tokenLineStart || lastCodeLine == invalidCodeLine);

            if (tokenLineStart == tokenLineEnd) {
                // In this case, token is on the same line. We just check,
                // if we counted this line in previous iteration and if not,
                // then count the line.
                if (lastCodeLine != tokenLineStart)
                    ++lineCodeCount;
            } else {
                // Just add the count of lines the token is on
                lineCodeCount += (tokenLineEnd - tokenLineStart);
                if (lastCodeLine != tokenLineStart)
                    ++lineCodeCount;
            }

            lastCodeLine = tokenLineEnd;
        }
        item->linesOfCode = lineCodeCount;
    }

    emit codeMetricsCollected(item);
}

} // namespace Internal
} // namespace CodeMetrics
