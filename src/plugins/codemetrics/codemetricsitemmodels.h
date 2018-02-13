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

#include <QList>
#include <QScopedPointer>
#include <QAbstractItemModel>

#include <utils/fileutils.h>
#include <utils/treemodel.h>

namespace CodeMetrics {
namespace Internal {

class ProjectTreeItem : public Utils::TypedTreeItem<ProjectTreeItem>
{
public:

    // Type of the tree item.
    enum Kind {
        Root,
        ProjectDirectory,
        HeadersDirectory,
        SourcesDirectory,
        HeaderFile,
        SourceFile,
        Summary
    };

    struct ItemData {
        Kind m_kind = Root;
        QString m_displayName;
        Utils::FileName m_file;
        int m_lines = -1;
        int m_linesOfCode = -1;
        int m_linesOfComment = -1;
        QIcon m_icon;

        void init();
        void merge(const ItemData& itemData);
    };

    explicit ProjectTreeItem(ItemData itemData) : m_itemData(qMove(itemData)) { }

    virtual QVariant data(int column, int role) const Q_DECL_OVERRIDE;

    void calculateDataFromChildren();

private:
    QVariant getDecorationData(int column) const;
    QVariant getDisplayData(int column) const;
    QVariant getToolTipData(int column) const;
    QVariant getSizeHintData(int column) const;
    QVariant getTextAlignmentData(int column) const;
    QVariant getSortData(int column) const;
    QVariant getIdData(int column) const;
    QVariant getExpandedByDefaultData(int column) const;

    ItemData m_itemData;
};

class ProjectTreeItemsModel : public Utils::TreeModel<ProjectTreeItem>
{
    Q_OBJECT

public:
    explicit ProjectTreeItemsModel(ProjectTreeItem *root, QObject *parent = Q_NULLPTR);

    enum Column {
        ProjectOrFileNameColumn,
        LinesColumn,
        LinesOfCodeColumn,
        LinesOfCommentsColumn,
        CommentToCodeRatioColumn,
        ColumnCount
    };

    // QAbstractItemModel interface
public:
    virtual int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

} // namespace Internal
} // namespace CodeMetrics
