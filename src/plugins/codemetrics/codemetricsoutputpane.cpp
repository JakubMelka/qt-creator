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

#include "codemetricsoutputpane.h"
#include "codemetricstreeview.h"
#include "codemetricsplugin.h"
#include "codemetricsitemmodels.h"
#include "constants.h"

#include <utils/utilsicons.h>
#include <aggregation/aggregate.h>
#include <coreplugin/find/itemviewfind.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QAction>
#include <QToolButton>
#include <QButtonGroup>
#include <QSortFilterProxyModel>

namespace CodeMetrics {
namespace Internal {

CodeMetricsOutputPane::CodeMetricsOutputPane(CodeMetricsPlugin *plugin, QObject* parent) :
    Core::IOutputPane(parent),
    m_plugin(plugin),
    m_treeView(Q_NULLPTR),
    m_expandCollapseButton(Q_NULLPTR),
    m_expandCollapseAction(Q_NULLPTR),
    m_scopeCurrentDocumentButton(Q_NULLPTR),
    m_scopeActiveProjectButton(Q_NULLPTR),
    m_scopeCurrentProjectButton(Q_NULLPTR),
    m_scopeButtons(Q_NULLPTR),
    m_filterProxyModel(Q_NULLPTR)
{
    connect(plugin, &CodeMetricsPlugin::settingsChanged, this, &CodeMetricsOutputPane::settingsChanged);
    initWidgets();
    settingsChanged();
}

CodeMetricsOutputPane::~CodeMetricsOutputPane()
{
    delete m_treeView;
    delete m_expandCollapseButton;
    delete m_expandCollapseAction;
    delete m_scopeButtons;
    delete m_scopeCurrentDocumentButton;
    delete m_scopeActiveProjectButton;
    delete m_scopeCurrentProjectButton;
}

QWidget *CodeMetricsOutputPane::outputWidget(QWidget *parent)
{
    Q_UNUSED(parent);
    return m_treeView;
}

QList<QWidget*> CodeMetricsOutputPane::toolBarWidgets() const
{
    QList<QWidget*> widgets;

    widgets << m_expandCollapseButton
            << m_scopeCurrentDocumentButton
            << m_scopeActiveProjectButton
            << m_scopeCurrentProjectButton;

    return widgets;
}

QString CodeMetricsOutputPane::displayName() const
{
    return tr(Constants::OUTPUT_PANE_TITLE);
}

int CodeMetricsOutputPane::priorityInStatusBar() const
{
    return 1;
}

void CodeMetricsOutputPane::clearContents()
{

}

void CodeMetricsOutputPane::visibilityChanged(bool visible)
{
    Q_UNUSED(visible);
}

void CodeMetricsOutputPane::setFocus()
{
    m_treeView->setFocus();
}

bool CodeMetricsOutputPane::hasFocus() const
{
    return m_treeView->hasFocus();
}

bool CodeMetricsOutputPane::canFocus() const
{
    return true;
}

bool CodeMetricsOutputPane::canNavigate() const
{
    return true;
}

bool CodeMetricsOutputPane::canNext() const
{
    return nextModelIndex().isValid();
}

bool CodeMetricsOutputPane::canPrevious() const
{
    return previousModelIndex().isValid();
}

void CodeMetricsOutputPane::goToNext()
{
    QModelIndex index = nextModelIndex();
    m_treeView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent
                                         | QItemSelectionModel::Rows
                                         | QItemSelectionModel::Clear);
}

void CodeMetricsOutputPane::goToPrev()
{
    QModelIndex index = previousModelIndex();
    m_treeView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent
                                         | QItemSelectionModel::Rows
                                         | QItemSelectionModel::Clear);
}

void CodeMetricsOutputPane::settingsChanged()
{
    const CodeMetricsSettings &settings = m_plugin->settings();

    switch (settings.scope) {
    case ScopeCurrentDocument:
        m_scopeCurrentDocumentButton->setChecked(true);
        break;
    case ScopeActiveProject:
        m_scopeActiveProjectButton->setChecked(true);
        break;
    case ScopeCurrentSubproject:
        m_scopeCurrentProjectButton->setChecked(true);
        break;
    }
}

void CodeMetricsOutputPane::scopeButtonClicked(QAbstractButton *button)
{
    CodeMetricsScope scope = ScopeActiveProject;
    if (button == m_scopeCurrentDocumentButton) {
        scope = ScopeCurrentDocument;
    } else if (button == m_scopeActiveProjectButton) {
        scope = ScopeActiveProject;
    } else if (button == m_scopeCurrentProjectButton) {
        scope = ScopeCurrentSubproject;
    } else {
        Q_ASSERT_X(false, "CodeMetricsOutputPane::scopeButtonClicked", "Unknown scope button clicked!");
    }

    CodeMetricsSettings settings = m_plugin->settings();
    settings.scope = scope;
    m_plugin->setSettings(settings);
}

void CodeMetricsOutputPane::expandCollapseActionToggled(bool checked)
{
    m_treeView->setAutoExpandTree(checked);
    if (checked) {
        m_expandCollapseAction->setText(tr("Collapse All"));
        m_treeView->expandAll();
    } else {
        m_expandCollapseAction->setText(tr("Expand All"));
        m_treeView->collapseAll();
    }
}

void CodeMetricsOutputPane::modelCreated(QSharedPointer<QAbstractItemModel> model)
{
    m_treeView->saveState();
    m_expandCollapseAction->setEnabled(!model.isNull());
    m_filterProxyModel->setObjectName(!model.isNull() ? QLatin1String(model->metaObject()->className()) : QString());
    m_filterProxyModel->setSourceModel(model.data());
    m_sourceModel = qMove(model);
    m_treeView->restoreState();
    emit navigateStateUpdate();
}

void CodeMetricsOutputPane::initWidgets()
{
    // Create tree view for displaying the code metrics
    m_treeView = new CodeMetricsTreeView(Q_NULLPTR);
    connect(m_treeView, &CodeMetricsTreeView::clicked, this, &CodeMetricsOutputPane::codeMetricsTreeViewClicked);

    // Create a proxy model for filtering/sorting
    m_filterProxyModel = new QSortFilterProxyModel(m_treeView);
    m_filterProxyModel->setSourceModel(Q_NULLPTR);
    m_filterProxyModel->setDynamicSortFilter(true);
    m_filterProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_filterProxyModel->setSortLocaleAware(true);
    m_filterProxyModel->setSortRole(Constants::SortRole);

    m_treeView->setModel(m_filterProxyModel);

    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &CodeMetricsOutputPane::navigateStateChanged);

    Aggregation::Aggregate *aggregate = new Aggregation::Aggregate(this);
    aggregate->add(m_treeView);
    aggregate->add(new Core::ItemViewFind(m_treeView, Qt::DisplayRole,
                                          Core::ItemViewFind::DoNotFetchMoreWhileSearching));

    m_scopeCurrentDocumentButton = new QToolButton(Q_NULLPTR);
    m_scopeCurrentDocumentButton->setCheckable(true);
    m_scopeCurrentDocumentButton->setText(tr("Current Document"));
    m_scopeCurrentDocumentButton->setToolTip(tr("Show code metrics for the currently edited document."));

    m_scopeActiveProjectButton = new QToolButton(Q_NULLPTR);
    m_scopeActiveProjectButton->setCheckable(true);
    m_scopeActiveProjectButton->setText(tr("Active Project"));
    m_scopeActiveProjectButton->setToolTip(tr("Show code metrics for the whole active project."));

    m_scopeCurrentProjectButton = new QToolButton(Q_NULLPTR);
    m_scopeCurrentProjectButton->setCheckable(true);
    m_scopeCurrentProjectButton->setText(tr("Subproject"));
    m_scopeCurrentProjectButton->setToolTip(tr("Show code metrics for the current subproject."));

    m_scopeButtons = new QButtonGroup(this);
    m_scopeButtons->addButton(m_scopeCurrentDocumentButton);
    m_scopeButtons->addButton(m_scopeActiveProjectButton);
    m_scopeButtons->addButton(m_scopeCurrentProjectButton);

    connect(m_scopeButtons, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
            this, &CodeMetricsOutputPane::scopeButtonClicked);

    m_expandCollapseAction = new QAction(Utils::Icons::EXPAND_ALL_TOOLBAR.icon(),
                                         tr("Expand All"), Q_NULLPTR);
    m_expandCollapseAction->setCheckable(true);
    Core::Command *command = Core::ActionManager::registerAction(m_expandCollapseAction, "CodeMetrics.ExpandAll");
    command->setAttribute(Core::Command::CA_UpdateText);

    m_expandCollapseButton = new QToolButton(Q_NULLPTR);
    m_expandCollapseButton->setAutoRaise(true);
    m_expandCollapseButton->setDefaultAction(command->action());

    connect(m_expandCollapseAction, &QAction::toggled,
            this, &CodeMetricsOutputPane::expandCollapseActionToggled);
}

void CodeMetricsOutputPane::codeMetricsTreeViewClicked(const QModelIndex &index)
{
    QVariant sourceLocation = index.data(Constants::SourceLocationRole);
    if (sourceLocation.isValid()) {
        SourceLocationData location = sourceLocation.value<SourceLocationData>();
        if (location.m_file.exists())
            Core::EditorManager::openEditorAt(location.m_file.toString(), location.m_line);
    }
}

QModelIndex CodeMetricsOutputPane::selectedIndex() const
{
    QModelIndexList selectedIndexes = m_treeView->selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty())
        return selectedIndexes.front();
    else
        return QModelIndex();
}

QModelIndex CodeMetricsOutputPane::nextModelIndex() const
{
    return m_treeView->indexBelow(selectedIndex());
}

QModelIndex CodeMetricsOutputPane::previousModelIndex() const
{
    return m_treeView->indexAbove(selectedIndex());
}

} // namespace Internal
} // namespace CodeMetrics
