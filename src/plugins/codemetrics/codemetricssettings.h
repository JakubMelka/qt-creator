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

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QSettings)

namespace CodeMetrics {
namespace Internal {

enum CodeMetricsScope {
    ScopeCurrentDocument,
    ScopeActiveProject,
    ScopeCurrentSubproject
};

class CodeMetricsSettings
{
public:
    CodeMetricsScope scope = ScopeActiveProject;

    // Maintainability sensitivity parameters. To compute maintainability
    // of the code, these values are used to compute the portions of the
    // maintainability. There are two variables, from which is computed
    // maintainability, cyclomatic complexity and instruction count.
    // If the sensitivity of the cyclomatic complexity is 5, then
    // the corresponding normal level maintainability is 15. Similarly,
    // if instructions sensitivity is 25, then 75 instructions are
    // regarded as normal level maintainability.
    int ccSensitivity = 5;
    int insSensitivity = 25;

    void save(QSettings *settings) const;
    void load(QSettings *settings);

    bool operator==(const CodeMetricsSettings &other) const;
    bool operator!=(const CodeMetricsSettings &other) const { return !(*this == other); }
};

} // namespace Internal
} // namespace CodeMetrics
