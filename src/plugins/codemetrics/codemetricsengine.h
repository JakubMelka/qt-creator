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
#include "codemetricssettings.h"

#include <projectexplorer/project.h>
#include <coreplugin/editormanager/ieditor.h>

#include <QHash>
#include <QMutex>
#include <QObject>

namespace CodeMetrics {
namespace Internal {

class CodeMetricsSettings;
class CodeMetricsCollector;
class CodeMetricsDataProcessor;

using CodeMetricsModelPointer = QSharedPointer<QAbstractItemModel>;

class CodeMetricsEngine : public QObject
{
    Q_OBJECT

public:
    explicit CodeMetricsEngine(QObject *parent = Q_NULLPTR);
    virtual ~CodeMetricsEngine();

    void setSettings(const CodeMetricsSettings &newSettings);

signals:
    // This signal is emitted, when data is successfully processed
    // and a new model is created. Model can also be null, in case
    // when no data were collected.
    void modelCreated(CodeMetricsModelPointer model);

private slots:
    void startupProjectChanged(ProjectExplorer::Project *project);
    void projectFilesChanged();
    void currentEditorChanged(Core::IEditor *editor);

private:
    void initCodeMetricsCollector();
    void freeCodeMetricsCollector();

    void initCodeMetricsDataProcessor();
    void freeCodeMetricsDataProcessor();

    enum UpdateMode {
        Immediate,
        Delayed
    };

    // Creates a request for update of the code metrics model.
    // The request can be dispatched immediately, or it can
    // be delayed by some time.
    void updateCodeMetricsModel(UpdateMode updateMode);

    // Creates a request for update of the code metrics model
    // and dispatches it to the data processor.
    void createCodeMetricsModelAsync();

    void codeMetricsCollected(const CodeMetricsItemPointer& codeMetrics);

    // Some operation with this object must be thread safe.
    // Storage for the items is accessed from multiple threads.
    QMutex m_mutex;

    // Stores code metrics for all files. Key is file name of the
    // document, value is collected code metrics for the document.
    // Access to this member of the class must be protected by
    // the mutex.
    QHash<Utils::FileName, CodeMetricsItemPointer> m_itemStorage;

    // This flag signalizes, that update is needed and new request
    // to the data processor is needed.
    bool m_updateNeeded;

    ProjectExplorer::Project *m_startupProject;
    Core::IEditor* m_currentEditor;

    CodeMetricsSettings m_settings;
    CodeMetricsCollector *m_codeMetricsCollector;
    CodeMetricsDataProcessor *m_codeMetricsDataProcessor;
};

} // namespace Internal
} // namespace CodeMetrics
