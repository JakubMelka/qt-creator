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

#include "codemetricssettings.h"
#include "constants.h"

#include <QtCore>
#include <QColor>
#include <QString>

namespace CodeMetrics {
namespace Internal {

// This class is used to compute maintainability from the code metrics,
// and to get text/background color for different maintainability levels.
class CodeMetricsMaintainability
{
    Q_DECLARE_TR_FUNCTIONS(CodeMetrics::Internal::CodeMetricsMaintainability)

public:

    // Calculates maintainability from the given code metrics
    static Constants::Maintainability calculate(int cyclomaticComplexity,
                                                int instructionCount,
                                                const CodeMetricsSettings& settings);

    static QString getText(const Constants::Maintainability maintainability);
    static QColor getBackgroundColor(const Constants::Maintainability maintainability);

private:
    CodeMetricsMaintainability() = delete;
};

} // namespace Internal
} // namespace CodeMetrics
