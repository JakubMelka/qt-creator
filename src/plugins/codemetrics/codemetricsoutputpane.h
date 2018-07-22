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

#include <coreplugin/ioutputpane.h>

QT_BEGIN_NAMESPACE
class QToolButton;
class QButtonGroup;
class QModelIndex;
class QAbstractButton;
class QAbstractItemModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

namespace CodeMetrics {
namespace Internal {

class CodeMetricsPlugin;
class CodeMetricsTreeView;

class CodeMetricsOutputPane : public Core::IOutputPane
{
    Q_OBJECT

public:
    CodeMetricsOutputPane(CodeMetricsPlugin *plugin, QObject *parent = Q_NULLPTR);
    ~CodeMetricsOutputPane();

    // IOutputPane interface
public:
    virtual QWidget *outputWidget(QWidget *parent) Q_DECL_OVERRIDE;
    virtual QList<QWidget*> toolBarWidgets() const Q_DECL_OVERRIDE;
    virtual QString displayName() const Q_DECL_OVERRIDE;
    virtual int priorityInStatusBar() const Q_DECL_OVERRIDE;
    virtual void clearContents() Q_DECL_OVERRIDE;
    virtual void visibilityChanged(bool visible) Q_DECL_OVERRIDE;
    virtual void setFocus() Q_DECL_OVERRIDE;
    virtual bool hasFocus() const Q_DECL_OVERRIDE;
    virtual bool canFocus() const Q_DECL_OVERRIDE;
    virtual bool canNavigate() const Q_DECL_OVERRIDE;
    virtual bool canNext() const Q_DECL_OVERRIDE;
    virtual bool canPrevious() const Q_DECL_OVERRIDE;
    virtual void goToNext() Q_DECL_OVERRIDE;
    virtual void goToPrev() Q_DECL_OVERRIDE;

public slots:
    void modelCreated(QSharedPointer<QAbstractItemModel> model);

private slots:
    void settingsChanged();
    void scopeButtonClicked(QAbstractButton *button);
    void expandCollapseActionToggled(bool checked);

private:
    void initWidgets();
    void codeMetricsTreeViewClicked(const QModelIndex &index);

    QModelIndex selectedIndex() const;
    QModelIndex nextModelIndex() const;
    QModelIndex previousModelIndex() const;

    CodeMetricsPlugin *m_plugin;
    CodeMetricsTreeView *m_treeView;
    QToolButton *m_expandCollapseButton;
    QAction *m_expandCollapseAction;
    QToolButton *m_scopeCurrentDocumentButton;
    QToolButton *m_scopeActiveProjectButton;
    QToolButton *m_scopeCurrentProjectButton;
    QButtonGroup *m_scopeButtons;
    QSortFilterProxyModel *m_filterProxyModel;
    QSharedPointer<QAbstractItemModel> m_sourceModel;
};

} // namespace Internal
} // namespace CodeMetrics
