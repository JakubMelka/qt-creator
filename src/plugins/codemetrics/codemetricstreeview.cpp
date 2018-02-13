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

#include "codemetricstreeview.h"
#include "constants.h"

#include <QHeaderView>

namespace CodeMetrics {
namespace Internal {


CodeMetricsTreeView::CodeMetricsTreeView(QWidget* parent) :
    Utils::TreeView(parent),
    m_autoExpandTree(false)
{
    setSortingEnabled(true);
    setRootIsDecorated(true);
    setFrameStyle(NoFrame);
    setSelectionBehavior(SelectRows);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setItemDelegate(new CodeMetricsTreeViewDelegate(this));

    QHeaderView *header = this->header();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setStretchLastSection(true);
    header->setSectionsMovable(false);

    sortByColumn(0, Qt::AscendingOrder);

    connect(this, &CodeMetricsTreeView::expanded, this, &CodeMetricsTreeView::onItemExpanded);
    connect(this, &CodeMetricsTreeView::collapsed, this, &CodeMetricsTreeView::onItemCollapsed);
}

CodeMetricsTreeView::~CodeMetricsTreeView()
{

}

void CodeMetricsTreeView::saveState()
{
    QString modelName = model()->objectName();
    if (!modelName.isEmpty()) {
        State state;
        state.m_headerState = header()->saveState();
        state.m_selectedItemId = getSelectedIndex().data(Constants::IdRole).toString();
        state.m_expandedItems = m_expandedItems;
        m_savedStates[modelName] = qMove(state);
    }
}

void CodeMetricsTreeView::restoreState()
{
    QString modelName = model()->objectName();
    if (!modelName.isEmpty()) {
        bool wasUpdatesEnabled = updatesEnabled();
        setUpdatesEnabled(false);

        State state = m_savedStates.value(modelName, State());

        // Restore header state, if it is present
        if (!state.m_headerState.isEmpty()) {
            header()->restoreState(state.m_headerState);
        } else {
            // Set default state of the header
            QHeaderView *header = this->header();
            for (int i = 0, count = header->count() - 1; i < count; ++i) {
                const int width = sizeHintForColumn(i);
                header->resizeSection(i, width);
            }
        }

        // Restore expanded items
        collapseAll();
        m_expandedItems.clear();

        QModelIndex selectedIndex;

        std::function<void(const QModelIndex&, const QAbstractItemModel*)> restoreExpandedState =
                [this, &restoreExpandedState, &state, &selectedIndex]
                (const QModelIndex &index, const QAbstractItemModel *model)
        {
            const int rowCount = model->rowCount(index);
            for (int i = 0; i < rowCount; ++i) {
                QModelIndex subIndex = model->index(i, 0, index);
                QString id = subIndex.data(Constants::IdRole).toString();

                if (!id.isEmpty()) {
                    // In this way we will find selected index
                    if (id == state.m_selectedItemId)
                        selectedIndex = subIndex;

                    // Expand item, if it was expanded, or use default expansion
                    if (state.m_expandedItems.contains(id)) {
                        const bool expanded = m_autoExpandTree || state.m_expandedItems[id];
                        m_expandedItems[id] = expanded;
                        setExpanded(subIndex, expanded);
                    } else if (model->hasChildren(subIndex)) {
                        // Use default expansion
                        const bool expandedByDefault = m_autoExpandTree ||
                                subIndex.data(Constants::ExpandedByDefaultRole).toBool();
                        m_expandedItems[id] = expandedByDefault;
                        setExpanded(subIndex, expandedByDefault);
                    }
                }

                restoreExpandedState(subIndex, model);
            }
        };
        restoreExpandedState(QModelIndex(), model());

        if (selectedIndex.isValid())
            this->scrollTo(selectedIndex, EnsureVisible);

        setUpdatesEnabled(wasUpdatesEnabled);
    }
}

void CodeMetricsTreeView::setAutoExpandTree(bool autoExpandTree)
{
    m_autoExpandTree = autoExpandTree;
}

QModelIndex CodeMetricsTreeView::getSelectedIndex() const
{
    QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty())
        return selectedIndexes.front();
    else
        return QModelIndex();
}

void CodeMetricsTreeView::onItemExpanded(const QModelIndex &index)
{
    QString id = index.data(Constants::IdRole).toString();
    if (!id.isEmpty()) {
        m_expandedItems[id] = true;
    }
}

void CodeMetricsTreeView::onItemCollapsed(const QModelIndex &index)
{
    QString id = index.data(Constants::IdRole).toString();
    if (!id.isEmpty()) {
        m_expandedItems[id] = false;
    }
}

CodeMetricsTreeViewDelegate::CodeMetricsTreeViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void CodeMetricsTreeViewDelegate::paint(QPainter *painter,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    QStyleOptionViewItem mutableOption(option);

    // In the first column, we want to display text from right (because
    // usually a file name or project name is displayed here).
    // For example, if we have long file name, 'codemetricstreeview.cpp',
    // we prefer to display '...ricstreeview.cpp'.
    mutableOption.textElideMode = index.column() == 0 ? Qt::ElideLeft : Qt::ElideRight;

    QStyledItemDelegate::paint(painter, mutableOption, index);
}

QSize CodeMetricsTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    // We get size hint from model to set column width properly
    // (the result size is default size expanded to the model size)
    QSize sizeFromModel = qvariant_cast<QSize>(index.data(Qt::SizeHintRole));

    QStyleOptionViewItem mutableOption(option);
    initStyleOption(&mutableOption, index);
    const QWidget *widget = mutableOption.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    QSize sizeFromStyle = style->sizeFromContents(QStyle::CT_ItemViewItem, &mutableOption, QSize(), widget);

    QSize sizeHint = sizeFromStyle;
    if (sizeFromModel.isValid())
        sizeHint = sizeHint.expandedTo(sizeFromModel);
    return sizeHint;
}

} // namespace Internal
} // namespace CodeMetrics
