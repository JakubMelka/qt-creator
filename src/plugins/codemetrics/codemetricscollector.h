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

#include <cpptools/cppmodelmanager.h>

namespace CodeMetrics {
namespace Internal {

class CodeMetricsCollector : public QObject
{
    Q_OBJECT

public:
    explicit CodeMetricsCollector(QObject *parent = Q_NULLPTR);

signals:
    void codeMetricsCollected(const CodeMetricsItemPointer& codeMetrics);

private:
    // All these functions must be reentrant
    void documentUpdated(CPlusPlus::Document::Ptr document);
    void collectCodeMetrics(CPlusPlus::Document::Ptr document);
};

} // namespace Internal
} // namespace CodeMetrics
