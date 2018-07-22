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

#include <utils/fileutils.h>

#include <QSharedPointer>

namespace CodeMetrics {
namespace Internal {

class CodeMetricsFunctionItem
{
public:
    explicit CodeMetricsFunctionItem() = default;

    void init();

    QString qualifiedFunctionName;
    int line = -1;
    int cyclomaticComplexity = -1;
    int instructions = -1;
    int lines = -1;
    int linesOfCode = -1;
    int linesOfComment = -1;
};

class CodeMetricsItem
{
public:
    explicit CodeMetricsItem() = default;
    explicit CodeMetricsItem(const Utils::FileName& file) : file(file) { }

    bool operator==(const CodeMetricsItem &other) const;
    inline bool operator !=(const CodeMetricsItem &other) const { return !(*this == other); }

    Utils::FileName file;
    int lines = -1;
    int linesOfCode = -1;
    int linesOfComment = -1;
    QVector<CodeMetricsFunctionItem> functions;
};

using CodeMetricsItemPointer = QSharedPointer<CodeMetricsItem>;

} // namespace Internal
} // namespace CodeMetrics
