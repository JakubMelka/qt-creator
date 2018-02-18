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


#include "codemetricsmaintainability.h"

namespace CodeMetrics {
namespace Internal {

Constants::Maintainability CodeMetricsMaintainability::calculate(int cyclomaticComplexity,
                                                                 int instructionCount,
                                                                 const CodeMetricsSettings &settings)
{
    const qreal ccNumerator = cyclomaticComplexity;
    const qreal inNumerator = instructionCount;

    const qreal ccDenominator = settings.ccSensitivity;
    const qreal inDenominator = settings.insSensitivity;

    auto sqr = [](const qreal value) -> const qreal
    {
        return value * value;
    };

    const qreal ccPart = static_cast<qreal>(0.5) * sqr(ccNumerator / ccDenominator);
    const qreal inPart = static_cast<qreal>(0.5) * sqr(inNumerator / inDenominator);

    const qreal result = qSqrt(ccPart + inPart);
    const int rounded = qRound(result);
    const int bounded = qBound<int>(Constants::Excellent, rounded, Constants::Appalling);

    return static_cast<Constants::Maintainability>(bounded);
}

QString CodeMetricsMaintainability::getText(const Constants::Maintainability maintainability)
{
    switch (maintainability) {
    case CodeMetrics::Constants::Invalid:
        return QString();
    case CodeMetrics::Constants::Excellent:
        return tr("Excellent");
    case CodeMetrics::Constants::Good:
        return tr("Good");
    case CodeMetrics::Constants::Normal:
        return tr("Normal");
    case CodeMetrics::Constants::Bad:
        return tr("Bad");
    case CodeMetrics::Constants::Appalling:
        return tr("Appalling");
    }

    Q_ASSERT(false);
    return QString();
}

QColor CodeMetricsMaintainability::getBackgroundColor(const Constants::Maintainability maintainability)
{
    switch (maintainability) {
    case CodeMetrics::Constants::Invalid:
        return QColor(Qt::white);
    case CodeMetrics::Constants::Excellent:
        return QColor(64, 255, 64);
    case CodeMetrics::Constants::Good:
        return QColor(184, 255, 64);
    case CodeMetrics::Constants::Normal:
        return QColor(255, 255, 64);
    case CodeMetrics::Constants::Bad:
        return QColor(255, 159, 64);
    case CodeMetrics::Constants::Appalling:
        return QColor(255, 64, 64);
    }

    Q_ASSERT(false);
    return QColor(Qt::white);
}

} // namespace Internal
} // namespace CodeMetrics

