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

#include "codemetricsitemmodels.h"
#include "codemetricsmaintainability.h"
#include "constants.h"

#include <QLocale>

namespace CodeMetrics {
namespace Internal {


ProjectTreeItemsModel::ProjectTreeItemsModel(ProjectTreeItem *root, QObject *parent) :
    Utils::TreeModel<ProjectTreeItem>(root, parent)
{
    QStringList header;
    header << tr("Project/File");
    header << tr("Maintainability");
    header << tr("Cyclomatic complexity");
    header << tr("Instructions");
    header << tr("Lines");
    header << tr("Lines of Code");
    header << tr("Lines of Comment");
    header << tr("Comment to Code Ratio");
    this->setHeader(qMove(header));
}

int ProjectTreeItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

Qt::ItemFlags ProjectTreeItemsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if (!hasChildren(index))
        flags |= Qt::ItemNeverHasChildren;

    return flags;
}

QVariant ProjectTreeItem::data(int column, int role) const
{
    switch (role) {
    case Qt::DecorationRole: return getDecorationData(column);
    case Qt::DisplayRole: return getDisplayData(column);
    case Qt::ToolTipRole: return getToolTipData(column);
    case Qt::SizeHintRole: return getSizeHintData(column);
    case Qt::TextAlignmentRole: return getTextAlignmentData(column);
    case Qt::BackgroundColorRole: return getBackgroundColorData(column);
    case Constants::SortRole: return getSortData(column);
    case Constants::IdRole: return getIdData(column);
    case Constants::ExpandedByDefaultRole: return getExpandedByDefaultData(column);
    case Constants::SourceLocationRole: return getSourceLocationData(column);
    }

    return QVariant();
}

void ProjectTreeItem::calculateDataFromChildren()
{
    switch (m_itemData.m_kind) {
    case Root:
    case ProjectDirectory:
    case SourcesDirectory:
    case HeadersDirectory:
        for (int i = 0, count = childCount(); i < count; ++i)
            childAt(i)->calculateDataFromChildren();

        m_itemData.init();
        for (int i = 0, count = childCount(); i < count; ++i) {
            m_itemData.merge(childAt(i)->m_itemData);
        }
        break;

    case SourceFile:
    case HeaderFile:
        m_itemData.m_cyclomaticComplexity = 0;
        m_itemData.m_instructions = 0;

        for (int i = 0, count = childCount(); i < count; ++i) {
            m_itemData.mergeFunctionData(childAt(i)->m_itemData);
        }

        // If we doesn't have any data, reset items to no data.
        if (m_itemData.m_cyclomaticComplexity == 0)
            m_itemData.m_cyclomaticComplexity = -1;

        if (m_itemData.m_instructions == 0)
            m_itemData.m_instructions = -1;

        break;

    default:
        break;
    }
}

QVariant ProjectTreeItem::getDecorationData(int column) const
{
    if (column == ProjectTreeItemsModel::ProjectOrFileNameColumn)
        return m_itemData.m_icon;

    return QVariant();
}

QVariant ProjectTreeItem::getDisplayData(int column) const
{
    const ProjectTreeItemsModel::Column typedColumn = static_cast<ProjectTreeItemsModel::Column>(column);
    switch (typedColumn) {
    case ProjectTreeItemsModel::ProjectOrFileNameColumn:
        if (!m_itemData.m_displayName.isEmpty())
            return m_itemData.m_displayName;
        else
            return m_itemData.m_file.fileName();

    case ProjectTreeItemsModel::MaintainabilityColumn:
        return CodeMetricsMaintainability::getText(m_itemData.m_maintainability);

    case ProjectTreeItemsModel::CyclomaticComplexityColumn:
        return m_itemData.m_cyclomaticComplexity > -1 ? QLocale::system().toString(m_itemData.m_cyclomaticComplexity) : QVariant();

    case ProjectTreeItemsModel::InstructionsColumn:
        return m_itemData.m_instructions > -1 ? QLocale::system().toString(m_itemData.m_instructions) : QVariant();

    case ProjectTreeItemsModel::LinesColumn:
        return m_itemData.m_lines > -1 ? QLocale::system().toString(m_itemData.m_lines) : QVariant();

    case ProjectTreeItemsModel::LinesOfCodeColumn:
        return m_itemData.m_linesOfCode > -1 ? QLocale::system().toString(m_itemData.m_linesOfCode) : QVariant();

    case ProjectTreeItemsModel::LinesOfCommentsColumn:
        return m_itemData.m_linesOfComment > -1 ? QLocale::system().toString(m_itemData.m_linesOfComment) : QVariant();

    case ProjectTreeItemsModel::CommentToCodeRatioColumn:
        if (m_itemData.m_linesOfCode > 0 && m_itemData.m_linesOfComment > 0) {
            const qreal ratio = static_cast<qreal>(m_itemData.m_linesOfComment) /
                    static_cast<qreal>(m_itemData.m_linesOfCode);
            return QLocale::system().toString(ratio, 'f', 3);
        }
        break;
    }

    return QVariant();
}

QVariant ProjectTreeItem::getToolTipData(int column) const
{
    if (column == ProjectTreeItemsModel::ProjectOrFileNameColumn) {
        return m_itemData.m_file.toString();
    }

    return QVariant();
}

QVariant ProjectTreeItem::getSizeHintData(int column) const
{
    if (column == ProjectTreeItemsModel::ProjectOrFileNameColumn)
        return QSize(450, 0);
    else
        return QSize(120, 0);
}

QVariant ProjectTreeItem::getTextAlignmentData(int column) const
{
    switch (column) {
    case ProjectTreeItemsModel::ProjectOrFileNameColumn:
        return int(Qt::AlignLeft | Qt::AlignVCenter);

    case ProjectTreeItemsModel::MaintainabilityColumn:
        return int(Qt::AlignCenter);

    default:
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
}

QVariant ProjectTreeItem::getSortData(int column) const
{
    // If the item is summary, we want to be at the end at all costs
    if (m_itemData.m_kind == Summary)
        return QVariant();

    const ProjectTreeItemsModel::Column typedColumn = static_cast<ProjectTreeItemsModel::Column>(column);
    switch (typedColumn) {
    case ProjectTreeItemsModel::ProjectOrFileNameColumn:
        return getDisplayData(column);

    case ProjectTreeItemsModel::MaintainabilityColumn:
        return static_cast<int>(m_itemData.m_maintainability);

    case ProjectTreeItemsModel::CyclomaticComplexityColumn:
        return m_itemData.m_cyclomaticComplexity;

    case ProjectTreeItemsModel::InstructionsColumn:
        return m_itemData.m_instructions;

    case ProjectTreeItemsModel::LinesColumn:
        return m_itemData.m_lines;

    case ProjectTreeItemsModel::LinesOfCodeColumn:
        return m_itemData.m_linesOfCode;

    case ProjectTreeItemsModel::LinesOfCommentsColumn:
        return m_itemData.m_linesOfComment;

    case ProjectTreeItemsModel::CommentToCodeRatioColumn:
        if (m_itemData.m_linesOfCode > 0 && m_itemData.m_linesOfComment > 0) {
            const qreal ratio = static_cast<qreal>(m_itemData.m_linesOfComment) /
                    static_cast<qreal>(m_itemData.m_linesOfCode);
            return ratio;
        }

        // We do not have ratio. For this reason, we want to move these values
        // at the top. So we return -1, because ratio can be in range [0, 1].
        return QVariant(static_cast<qreal>(-1));
    }

    return QVariant();
}

QVariant ProjectTreeItem::getIdData(int column) const
{
    switch (m_itemData.m_kind) {
    case ProjectTreeItem::Root:
        return QString("$$root");
    case ProjectTreeItem::ProjectDirectory:
        return m_itemData.m_file.toString();
    case ProjectTreeItem::HeadersDirectory:
        return QString("%1/$$headers").arg(parent()->data(column, Constants::IdRole).toString());
    case ProjectTreeItem::SourcesDirectory:
        return QString("%1/$$sources").arg(parent()->data(column, Constants::IdRole).toString());

    case ProjectTreeItem::HeaderFile:
    case ProjectTreeItem::SourceFile:
        return m_itemData.m_file.toString();

    case ProjectTreeItem::Summary:
        return QString("$$summary");
    }

    return QVariant();
}

QVariant ProjectTreeItem::getExpandedByDefaultData(int column) const
{
    Q_UNUSED(column);

    switch (m_itemData.m_kind) {
    case ProjectTreeItem::Root:
        return true;

    case ProjectTreeItem::ProjectDirectory:
        // We have single project - expand it by default
        return parent()->childCount() == 1;

    case ProjectTreeItem::HeadersDirectory:
    case ProjectTreeItem::SourcesDirectory:
        // Expand automatically, if we have just one child
        return childCount() == 1;

    case ProjectTreeItem::HeaderFile:
    case ProjectTreeItem::SourceFile:
    case ProjectTreeItem::Summary:
    case ProjectTreeItem::Function:
        return false;
    }

    return QVariant();
}

QVariant ProjectTreeItem::getBackgroundColorData(int column) const
{
    if (column == ProjectTreeItemsModel::MaintainabilityColumn)
        return CodeMetricsMaintainability::getBackgroundColor(m_itemData.m_maintainability);

    return QColor(Qt::white);
}

QVariant ProjectTreeItem::getSourceLocationData(int column) const
{
    Q_UNUSED(column);

    switch (m_itemData.m_kind) {
    case ProjectTreeItem::Root:
    case ProjectTreeItem::ProjectDirectory:
    case ProjectTreeItem::HeadersDirectory:
    case ProjectTreeItem::SourcesDirectory:
    case ProjectTreeItem::Summary:
        return QVariant();

    case ProjectTreeItem::HeaderFile:
    case ProjectTreeItem::SourceFile:
        return QVariant::fromValue(SourceLocationData(m_itemData.m_file, 0));

    case ProjectTreeItem::Function: {
        SourceLocationData parentLocation = parent()->data(column, Constants::SourceLocationRole).value<SourceLocationData>();
        if (!parentLocation.m_file.isEmpty())
            return QVariant::fromValue(SourceLocationData(parentLocation.m_file, m_itemData.m_line));
        else
            return QVariant();
    }
    }

    return QVariant();
}

void ProjectTreeItem::ItemData::init()
{
    m_maintainability = Constants::Invalid;
    m_cyclomaticComplexity = 0;
    m_instructions = 0;
    m_line = 0;
    m_lines = 0;
    m_linesOfCode = 0;
    m_linesOfComment = 0;
}

void ProjectTreeItem::ItemData::mergeFunctionData(const ProjectTreeItem::ItemData &itemData)
{
    m_maintainability = qMax(m_maintainability, itemData.m_maintainability);
    m_cyclomaticComplexity += itemData.m_cyclomaticComplexity;
    m_instructions += itemData.m_instructions;
}

void ProjectTreeItem::ItemData::merge(const ProjectTreeItem::ItemData &itemData)
{
    m_lines += itemData.m_lines;
    m_linesOfCode += itemData.m_linesOfCode;
    m_linesOfComment += itemData.m_linesOfComment;

    mergeFunctionData(itemData);
}

} // namespace Internal
} // namespace CodeMetrics