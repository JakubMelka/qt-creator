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

#include "codemetricsitem.h"
#include "codemetricssettings.h"

#include "utils/fileutils.h"

#include <QHash>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSharedPointer>
#include <QAbstractItemModel>

namespace CodeMetrics {
namespace Internal {

Q_CONSTEXPR int PROCESSOR_TIMEOUT = 1000;

// Processor, which analyzes collected data and builds a model
// from analyzed data to present them in the GUI. All public functions
// of this class are protected by a mutex and are thread safe.
class CodeMetricsDataProcessor : public QThread
{
    Q_OBJECT

public:
    explicit CodeMetricsDataProcessor(QObject *parent = Q_NULLPTR);

    void startProcessor();
    void stopProcessor();

    struct Request
    {
        // This map contains child-parent relation of the projects.
        // If the project is not listed here, it is probably a root project.
        QHash<Utils::FileName, Utils::FileName> projectToProjectParent;

        // This hash map contains mapping of project to their
        // display name (which is shown in the tree).
        QHash<Utils::FileName, QString> projectToDisplayName;

        struct ProjectInfo
        {
            Utils::FileName project;
            Utils::FileNameList headers;
            Utils::FileNameList sources;
        };

        QList<ProjectInfo> projects;

        // This storage contains items selected by headers/sources
        // of the active projects (collected in the projects info).
        QHash<Utils::FileName, CodeMetricsItemPointer> selection;

        // Actual settings selected by user
        CodeMetricsSettings settings;
    };

    void setRequest(Request request);

    enum DataProcessorState {
        Inactive,   // Data processor is inactive (no thread running)
        Ready,      // Data processor is ready waiting for work
        Update,     // Data processor should analyze the data
        Finishing   // Data processor should finish the work and quit
    };

signals:
    // This signal is emitted, when data is successfully processed
    // and a new model is created. Model can also be null, in case
    // when no data were collected.
    void modelCreated(QSharedPointer<QAbstractItemModel> model);

protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    bool isFinishing() const { return m_state == Finishing; }

    void processRequest(const Request& request);

    // Mutex for protecting multiple thread access to this object,
    // also use for wait condition.
    QMutex m_mutex;

    // Wait condition, on which is thread waiting, while it is not working.
    QWaitCondition m_waitCondition;

    // Current state of the processor. It can be Ready (no work is performed),
    // Update (update request to analyze the data) or Finishing, which signalizes
    // that this processor should finish the work.
    DataProcessorState m_state;

    // Actual request for data processing
    Request m_request;
};

} // namespace Internal
} // namespace CodeMetrics
