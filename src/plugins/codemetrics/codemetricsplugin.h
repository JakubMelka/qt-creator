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

#include <extensionsystem/iplugin.h>

namespace CodeMetrics {
namespace Internal {

class CodeMetricsEngine;
class CodeMetricsOutputPane;

// FIXME: Change license to GPL
// FIXME: Fix GCC compilation
class CodeMetricsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "CodeMetrics.json")

public:
    CodeMetricsPlugin();
    ~CodeMetricsPlugin();

    void extensionsInitialized();
    bool initialize(const QStringList &arguments, QString *errorString);

    const CodeMetricsSettings& settings() const { return m_settings; }
    void setSettings(const CodeMetricsSettings &settings);

signals:
    void settingsChanged();

private:
    void createEngine();
    void createOutputPane();

    void loadSettings();
    void saveSettings();

    CodeMetricsSettings m_settings;
    CodeMetricsEngine *m_engine;
    CodeMetricsOutputPane *m_outputPane;
};

} // namespace Internal
} // namespace CodeMetrics
