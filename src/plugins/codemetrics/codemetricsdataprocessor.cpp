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

#include "codemetricsdataprocessor.h"
#include "codemetricsitemmodels.h"
#include "constants.h"

#include <coreplugin/fileiconprovider.h>

namespace CodeMetrics {
namespace Internal {

CodeMetricsDataProcessor::CodeMetricsDataProcessor(QObject *parent) :
    QThread(parent),
    m_state(Inactive)
{

}

void CodeMetricsDataProcessor::startProcessor()
{
    QMutexLocker lock(&m_mutex);
    if (m_state == Inactive) {
        m_state = Ready;
        start(LowPriority);
    }
}

void CodeMetricsDataProcessor::stopProcessor()
{
    QMutexLocker lock(&m_mutex);
    if (m_state != Inactive) {
        m_state = Finishing;
        lock.unlock();
        wait();
    }
}

void CodeMetricsDataProcessor::setRequest(Request request)
{
    QMutexLocker lock(&m_mutex);
    switch (m_state) {
    case Inactive:
        // Do nothing, processor is inactive
        break;

    case Ready:
        m_state = Update;
        m_request = qMove(request);
        m_waitCondition.wakeOne();
        break;

    case Update:
        m_request = qMove(request);
        m_waitCondition.wakeOne();
        break;

    case Finishing:
        // Again do nothing. Processor is finishing.
        break;
    }
}

void CodeMetricsDataProcessor::run()
{
    QMutexLocker lock(&m_mutex);

    while (!isFinishing()) {
        // We are waiting with timeout. Unfortunately, some version of the Qt's
        // QWaitCondition contains a bug, which will cause potential hangup.
        // The "wait" condition sometimes is not atomic.
        m_waitCondition.wait(&m_mutex, PROCESSOR_TIMEOUT);

        // Now, we have locked mutex. We can access data.
        while (m_state == Update) {
            // We analyze the data with unlocked mutex. Locked mutex can
            // cause GUI hangups. Side effect is, that multiple rework
            // can be done. But the work is done in the non-gui thread,
            // so it is acceptible.
            m_state = Ready;
            Request request = qMove(m_request);
            lock.unlock();

            processRequest(request);

            lock.relock();
            // After the relock, state can be changed. The finish can be requested,
            // or a new analyze round can be performed.
        }
    }

    // Set the state to inactive.
    m_state = Inactive;
}

void CodeMetricsDataProcessor::processRequest(const CodeMetricsDataProcessor::Request &request)
{
    if (request.selection.isEmpty()) {
        emit modelCreated(nullptr);
        return;
    }

    ProjectTreeItem *root = new ProjectTreeItem(ProjectTreeItem::ItemData());
    QSharedPointer<QAbstractItemModel> model(new ProjectTreeItemsModel(root, nullptr));

    QHash<Utils::FileName, ProjectTreeItem*> projectToProjectTreeItem;
    QHash<Utils::FileName, ProjectTreeItem*> projectHeadersRoot;
    QHash<Utils::FileName, ProjectTreeItem*> projectSourcesRoot;

    std::function<void(const Utils::FileName&)> addProjectTreeItemForProject =
            [&addProjectTreeItemForProject, &request, &projectToProjectTreeItem,
            &projectHeadersRoot, &projectSourcesRoot, root]
            (const Utils::FileName& project)
    {
        if (project.isEmpty() || projectToProjectTreeItem.contains(project))
            return;

        // First, insert parent project, if it exists.
        ProjectTreeItem *parentProjectTreeItem = root;
        Utils::FileName parentProject = request.projectToProjectParent.value(project, Utils::FileName());
        if (!parentProject.isEmpty()) {
            addProjectTreeItemForProject(parentProject);
            parentProjectTreeItem = projectToProjectTreeItem.value(parentProject);
        }

        ProjectTreeItem::ItemData projectData;
        projectData.m_file = project;
        projectData.m_kind = ProjectTreeItem::ProjectDirectory;
        projectData.m_displayName = request.projectToDisplayName.value(project);
        projectData.m_icon = Core::FileIconProvider::directoryIcon(Constants::ICONOVERLAY_QT);
        ProjectTreeItem *projectTreeItem = new ProjectTreeItem(qMove(projectData));
        parentProjectTreeItem->appendChild(projectTreeItem);
        projectToProjectTreeItem[project] = projectTreeItem;

        ProjectTreeItem::ItemData headersFolderData;
        headersFolderData.m_displayName = tr("Headers");
        headersFolderData.m_kind = ProjectTreeItem::HeadersDirectory;
        headersFolderData.m_icon = Core::FileIconProvider::directoryIcon(Constants::ICONOVERLAY_H);
        ProjectTreeItem *headersTreeItem = new ProjectTreeItem(qMove(headersFolderData));
        projectTreeItem->appendChild(headersTreeItem);
        projectHeadersRoot[project] = headersTreeItem;

        ProjectTreeItem::ItemData sourcesFolderData;
        sourcesFolderData.m_displayName = tr("Sources");
        sourcesFolderData.m_kind = ProjectTreeItem::SourcesDirectory;
        sourcesFolderData.m_icon = Core::FileIconProvider::directoryIcon(Constants::ICONOVERLAY_CPP);
        ProjectTreeItem *sourcesTreeItem = new ProjectTreeItem(qMove(sourcesFolderData));
        projectTreeItem->appendChild(sourcesTreeItem);
        projectSourcesRoot[project] = sourcesTreeItem;
    };

    auto processFile = [&request](const Utils::FileName& fileName, ProjectTreeItem *parent, bool isHeader)
    {
        auto it = request.selection.find(fileName);
        if (it != request.selection.cend()) {
            const CodeMetricsItem *codeMetrics = it.value().data();

            ProjectTreeItem::ItemData data;
            data.m_file = codeMetrics->file;
            data.m_kind = isHeader ? ProjectTreeItem::HeaderFile : ProjectTreeItem::HeaderFile;
            data.m_lines = codeMetrics->lines;
            data.m_linesOfCode = codeMetrics->linesOfCode;
            data.m_linesOfComment = codeMetrics->linesOfComment;
            data.m_icon = Core::FileIconProvider::icon(codeMetrics->file.toFileInfo());

            parent->appendChild(new ProjectTreeItem(qMove(data)));
        }
    };

    // Iterate trough all projects, and create project tree.
    for (const Request::ProjectInfo& projectInfo : request.projects) {
        addProjectTreeItemForProject(projectInfo.project);

        // Process all header files for this project
        ProjectTreeItem *headersRoot = projectHeadersRoot.value(projectInfo.project);
        for (const Utils::FileName& headerFile : projectInfo.headers) {
            processFile(headerFile, headersRoot, true);
        }

        // Process all source files for this project
        ProjectTreeItem *sourcesRoot = projectSourcesRoot.value(projectInfo.project);
        for (const Utils::FileName& sourceFile : projectInfo.sources) {
            processFile(sourceFile, sourcesRoot, false);
        }
    }

    root->calculateDataFromChildren();

    emit modelCreated(qMove(model));
}

} // namespace Internal
} // namespace CodeMetrics
