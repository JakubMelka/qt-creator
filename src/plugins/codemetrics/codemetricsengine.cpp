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

#include "codemetricsengine.h"
#include "codemetricscollector.h"
#include "codemetricsdataprocessor.h"
#include "constants.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/idocument.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/session.h>

#include <QTimer>

namespace CodeMetrics {
namespace Internal {

CodeMetricsEngine::CodeMetricsEngine(QObject *parent) :
    QObject(parent),
    m_updateNeeded(false),
    m_startupProject(ProjectExplorer::SessionManager::startupProject()),
    m_currentEditor(Core::EditorManager::currentEditor()),
    m_codeMetricsCollector(Q_NULLPTR),
    m_codeMetricsDataProcessor(Q_NULLPTR)
{
    qRegisterMetaType<CodeMetricsModelPointer>("CodeMetricsModelPointer");

    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::startupProjectChanged,
            this, &CodeMetricsEngine::startupProjectChanged);
    connect(ProjectExplorer::ProjectExplorerPlugin::instance(), &ProjectExplorer::ProjectExplorerPlugin::fileListChanged,
            this, &CodeMetricsEngine::projectFilesChanged);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &CodeMetricsEngine::currentEditorChanged);

    initCodeMetricsCollector();
    initCodeMetricsDataProcessor();
}

CodeMetricsEngine::~CodeMetricsEngine()
{
    freeCodeMetricsCollector();
    freeCodeMetricsDataProcessor();
}

void CodeMetricsEngine::setSettings(const CodeMetricsSettings &newSettings)
{
    if (m_settings != newSettings) {
        m_settings = newSettings;
        updateCodeMetricsModel(Immediate);
    }
}

void CodeMetricsEngine::startupProjectChanged(ProjectExplorer::Project *project)
{
    if (m_startupProject != project) {
        m_startupProject = project;

        // In case of the current document scope, we do not update
        // model here (it depends on the editor).
        switch (m_settings.scope) {
        case ScopeActiveProject:
        case ScopeCurrentSubproject:
            if (project && !project->isParsing())
                updateCodeMetricsModel(Immediate);
            else
                updateCodeMetricsModel(Delayed);

        default:
            break;
        }
    }
}

void CodeMetricsEngine::projectFilesChanged()
{
    // We must update the whole model, because some
    // files can be removed/added and we do not know,
    // which one.
    updateCodeMetricsModel(Immediate);
}

void CodeMetricsEngine::currentEditorChanged(Core::IEditor *editor)
{
    if (m_currentEditor != editor) {
        m_currentEditor = editor;

        // In scopes other than current document/current subproject, we do not need
        // update - content doesn't depend on the current editor.
        switch (m_settings.scope) {
        case ScopeCurrentDocument:
        case ScopeCurrentSubproject:
            updateCodeMetricsModel(Immediate);

        default:
            break;
        }
    }
}

void CodeMetricsEngine::initCodeMetricsCollector()
{
    m_codeMetricsCollector = new CodeMetricsCollector(this);
    connect(m_codeMetricsCollector, &CodeMetricsCollector::codeMetricsCollected,
            this, &CodeMetricsEngine::codeMetricsCollected, Qt::DirectConnection);
}

void CodeMetricsEngine::freeCodeMetricsCollector()
{
    disconnect(m_codeMetricsCollector, &CodeMetricsCollector::codeMetricsCollected,
               this, &CodeMetricsEngine::codeMetricsCollected);
    delete m_codeMetricsCollector;
    m_codeMetricsCollector = Q_NULLPTR;
}

void CodeMetricsEngine::initCodeMetricsDataProcessor()
{
    m_codeMetricsDataProcessor = new CodeMetricsDataProcessor(this);
    connect(m_codeMetricsDataProcessor, &CodeMetricsDataProcessor::modelCreated,
            this, &CodeMetricsEngine::modelCreated);
    m_codeMetricsDataProcessor->startProcessor();
}

void CodeMetricsEngine::freeCodeMetricsDataProcessor()
{
    m_codeMetricsDataProcessor->stopProcessor();
    delete m_codeMetricsDataProcessor;
    m_codeMetricsDataProcessor = Q_NULLPTR;
}

void CodeMetricsEngine::updateCodeMetricsModel(UpdateMode updateMode)
{
    QMutexLocker lock(&m_mutex);
    m_updateNeeded = true;

    lock.unlock();
    if (updateMode == Immediate) {
        createCodeMetricsModelAsync();
    } else {
        QTimer::singleShot(Constants::DATA_PROCESSOR_REQUEST_DELAY, this, &CodeMetricsEngine::createCodeMetricsModelAsync);
    }
}

void CodeMetricsEngine::createCodeMetricsModelAsync()
{
    QMutexLocker lock(&m_mutex);
    if (m_updateNeeded) {
        // Mark the update as not needed - we create a new request for the actual
        // state of the data. It is protected by mutex.
        m_updateNeeded = false;

        CodeMetricsDataProcessor::Request request;

        auto addProject = [&request](const ProjectExplorer::ProjectNode *projectNode)
        {
            while (projectNode) {
                const Utils::FileName& filePath = projectNode->filePath();

                if (!request.projectToDisplayName.contains(filePath)) {
                    QString displayName = projectNode->displayName();
                    request.projectToDisplayName[filePath] = qMove(displayName);

                    // Move to the parent. We must also establish a connection
                    // between child and parent project.
                    projectNode = projectNode->parentProjectNode();
                    if (projectNode) {
                        request.projectToProjectParent[filePath] = projectNode->filePath();
                    }
                } else {
                    // We have already processed this project. Stop.
                    break;
                }
            }
        };

        // If this project isn't nullptr, we
        const ProjectExplorer::Project *projectToParse = Q_NULLPTR;

        switch (m_settings.scope) {
        case ScopeCurrentDocument:
        {
            if (m_currentEditor) {
                const Utils::FileName& fileName = m_currentEditor->document()->filePath();
                const ProjectExplorer::Node *node = ProjectExplorer::ProjectTree::nodeForFile(fileName);

                if (node && node->nodeType() == ProjectExplorer::NodeType::File) {
                    const ProjectExplorer::ProjectNode *projectNode = node->parentProjectNode();
                    if (projectNode) {
                        addProject(projectNode);
                        const ProjectExplorer::FileNode *fileNode = node->asFileNode();

                        CodeMetricsDataProcessor::Request::ProjectInfo projectInfo;
                        projectInfo.project = projectNode->filePath();

                        // Check, if the file is a header/source and insert it
                        // into the appropriate info.
                        switch (fileNode->fileType()) {
                        case ProjectExplorer::FileType::Header:
                            projectInfo.headers.append(fileNode->filePath());
                            request.projects.append(qMove(projectInfo));
                            break;

                        case ProjectExplorer::FileType::Source:
                            projectInfo.sources.append(fileNode->filePath());
                            request.projects.append(qMove(projectInfo));
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
            break;
        }

        case ScopeActiveProject:
            projectToParse = m_startupProject;
            break;

        case ScopeCurrentSubproject: {
            // Well, we will first try to get the current subproject from the editor. If we fail,
            // we try to obtain it from the current node in the project tree.
            if (m_currentEditor) {
                const Utils::FileName& filePath = m_currentEditor->document()->filePath();
                ProjectExplorer::Node *node = ProjectExplorer::ProjectTree::nodeForFile(filePath);
                projectToParse = ProjectExplorer::ProjectTree::projectForNode(node);
            }

            if (!projectToParse) {
                // We try to get the current project from the project tree
                projectToParse = ProjectExplorer::ProjectTree::currentProject();
            }

            break;
        } // case
        } // switch

        if (projectToParse && projectToParse->rootProjectNode()) {
            ProjectExplorer::ProjectNode *projectNode = projectToParse->rootProjectNode();
            addProject(projectNode);

            QHash<Utils::FileName, CodeMetricsDataProcessor::Request::ProjectInfo> projectInfo;

            auto processNode = [&addProject, &projectInfo](const ProjectExplorer::Node *node)
            {
                if (const ProjectExplorer::ProjectNode *subprojectNode = node->asProjectNode()) {
                    // Add a new subproject (root project is added above)
                    addProject(subprojectNode);
                } else if (const ProjectExplorer::FileNode *fileNode = node->asFileNode()) {
                    // We only process non-generated files
                    if (node->isGenerated())
                        return;

                    const ProjectExplorer::FileType fileType = fileNode->fileType();
                    switch (fileType) {
                    case ProjectExplorer::FileType::Source:
                        projectInfo[node->parentProjectNode()->filePath()].sources.append(fileNode->filePath());
                        break;

                    case ProjectExplorer::FileType::Header:
                        projectInfo[node->parentProjectNode()->filePath()].headers.append(fileNode->filePath());
                        break;

                    default:
                        break;
                    }
                }
            };
            projectNode->forEachGenericNode(processNode);

            // We must fix project names in the map. They are empty.
            QMutableHashIterator<Utils::FileName, CodeMetricsDataProcessor::Request::ProjectInfo> it(projectInfo);
            while (it.hasNext()) {
                it.next();
                it.value().project = it.key();
            }

            request.projects = projectInfo.values();
        }

        // Reserve the same size as the item storage. This is important, so we dont
        // reallocate memory much often.
        request.selection.reserve(m_itemStorage.size());

        for (const CodeMetricsDataProcessor::Request::ProjectInfo& projectInfo : request.projects) {
            // Insert all header file collected metrics
            for (const Utils::FileName& headerFile : projectInfo.headers) {
                if (request.selection.contains(headerFile))
                    continue;

                auto it = m_itemStorage.find(headerFile);
                if (it != m_itemStorage.cend()) {
                    request.selection.insert(it.key(), it.value());
                }
            }

            // Insert all source file collected metrics
            for (const Utils::FileName& sourceFile : projectInfo.sources) {
                if (request.selection.contains(sourceFile))
                    continue;

                auto it = m_itemStorage.find(sourceFile);
                if (it != m_itemStorage.cend()) {
                    request.selection.insert(it.key(), it.value());
                }
            }
        }

        // Set active settings to the request
        request.settings = m_settings;

        // Now, we have prepared the data. We can pass them to the data processor.
        m_codeMetricsDataProcessor->setRequest(qMove(request));
    }
}

void CodeMetricsEngine::codeMetricsCollected(const CodeMetricsItemPointer& codeMetrics)
{
    QMutexLocker lock(&m_mutex);
    const Utils::FileName& key = codeMetrics->file;
    if (!m_itemStorage.contains(key) || *m_itemStorage[key] != *codeMetrics) {
        m_itemStorage[key] = codeMetrics;
        lock.unlock();
        updateCodeMetricsModel(Delayed);
    }
}

} // namespace Internal
} // namespace CodeMetrics
