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

#include <utils/itemviews.h>

#include <QStyledItemDelegate>

namespace CodeMetrics {
namespace Internal {

class CodeMetricsTreeViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CodeMetricsTreeViewDelegate(QObject *parent = Q_NULLPTR);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

class CodeMetricsTreeView : public Utils::TreeView
{
    Q_OBJECT

public:
    explicit CodeMetricsTreeView(QWidget *parent = Q_NULLPTR);
    virtual ~CodeMetricsTreeView();

    void saveState();
    void restoreState();

    void setAutoExpandTree(bool autoExpandTree);

private:
    QModelIndex getSelectedIndex() const;

    void onItemExpanded(const QModelIndex &index);
    void onItemCollapsed(const QModelIndex &index);

    struct State
    {
        QByteArray m_headerState;
        QString m_selectedItemId;
        QHash<QString, bool> m_expandedItems;
    };

    // This map maps source model name to the saved state of the treeview.
    QMap<QString, State> m_savedStates;

    // This map maps id of the index to the expanded state. True means
    // it is expanded, false means it was collapsed.
    QHash<QString, bool> m_expandedItems;

    // Auto expand newly added items during model reset (in save/restore
    // state functions).
    bool m_autoExpandTree;
};

} // namespace Internal
} // namespace CodeMetrics
