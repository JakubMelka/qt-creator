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

#include "codemetricssettings.h"
#include "constants.h"

#include <QSettings>

namespace CodeMetrics {
namespace Internal {

void CodeMetricsSettings::save(QSettings *settings) const
{
    settings->beginGroup(Constants::SETTINGS_GROUP);
    settings->setValue(Constants::SCOPE, scope);
    settings->endGroup();
}

void CodeMetricsSettings::load(QSettings* settings)
{
    settings->beginGroup(Constants::SETTINGS_GROUP);

    QVariant scopeVariant = settings->value(Constants::SCOPE, ScopeActiveProject);
    scope = static_cast<CodeMetricsScope>(scopeVariant.toInt());

    settings->endGroup();
}

bool CodeMetricsSettings::operator==(const CodeMetricsSettings &other) const
{
    return scope == other.scope;
}

} // namespace Internal
} // namespace CodeMetrics
