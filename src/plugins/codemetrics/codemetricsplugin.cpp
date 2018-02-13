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

#include "codemetricsplugin.h"
#include "codemetricsengine.h"
#include "codemetricsoutputpane.h"

#include <coreplugin/icore.h>

namespace CodeMetrics {
namespace Internal {

CodeMetricsPlugin::CodeMetricsPlugin() :
    m_engine(Q_NULLPTR),
    m_outputPane(Q_NULLPTR)
{

}

CodeMetricsPlugin::~CodeMetricsPlugin()
{
}

bool CodeMetricsPlugin::initialize(const QStringList &args, QString *errMsg)
{
    Q_UNUSED(args);
    Q_UNUSED(errMsg);

    loadSettings();

    createEngine();
    createOutputPane();

    connect(Core::ICore::instance(), &Core::ICore::saveSettingsRequested,
            this, &CodeMetricsPlugin::saveSettings);
    connect(m_engine, &CodeMetricsEngine::modelCreated,
            m_outputPane, &CodeMetricsOutputPane::modelCreated);

    return true;
}

void CodeMetricsPlugin::createEngine()
{
    m_engine = new CodeMetricsEngine(Q_NULLPTR);
    m_engine->setSettings(m_settings);
    addAutoReleasedObject(m_engine);
}

void CodeMetricsPlugin::createOutputPane()
{
    m_outputPane = new CodeMetricsOutputPane(this, Q_NULLPTR);
    addAutoReleasedObject(m_outputPane);
}

void CodeMetricsPlugin::loadSettings()
{
    CodeMetricsSettings settings;
    settings.load(Core::ICore::settings(QSettings::UserScope));
    setSettings(settings);
}

void CodeMetricsPlugin::saveSettings()
{
    m_settings.save(Core::ICore::settings(QSettings::UserScope));
}

void CodeMetricsPlugin::setSettings(const CodeMetricsSettings &settings)
{
    if (m_settings != settings) {
        m_settings = settings;
        if (m_engine)
            m_engine->setSettings(m_settings);
        emit settingsChanged();
    }
}

void CodeMetricsPlugin::extensionsInitialized()
{

}

} // namespace Internal
} // namespace CodeMetrics
