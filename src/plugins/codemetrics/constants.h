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

#include <QtGlobal>
#include <QtCore>

namespace CodeMetrics {
namespace Constants {

const char OUTPUT_PANE_TITLE[] = QT_TRANSLATE_NOOP("CodeMetrics::Internal::CodeMetricsOutputPane", "Code Metrics");

Q_CONSTEXPR char SETTINGS_GROUP[] = "CodeMetricsPlugin";
Q_CONSTEXPR char SCOPE[] = "Scope";
Q_CONSTEXPR char CYCLOMATIC_COMPLEXITY_SENSITIVITY[] = "CC_Sensitivity";
Q_CONSTEXPR char INSTRUCTIONS_SENSITIVITY[] = "INS_Sensitivity";

Q_CONSTEXPR int DATA_PROCESSOR_REQUEST_DELAY = 1000;

Q_CONSTEXPR char ICONOVERLAY_QT[]   = ":/projectexplorer/images/fileoverlay_qt.png";
Q_CONSTEXPR char ICONOVERLAY_CPP[]  = ":/projectexplorer/images/fileoverlay_cpp.png";
Q_CONSTEXPR char ICONOVERLAY_H[]    = ":/projectexplorer/images/fileoverlay_h.png";

// This enum is used for item models
enum UserRoles {

    // Sorting role. Used by models for item sorting. Sorting
    // is sometimes not based on Qt::DisplayRole, because we do not
    // want string-based sorting, some columns can contain numbers.
    SortRole = Qt::UserRole,

    // This role represents unique identifier for the data in the underlying model.
    // It can be for example path to the file, if the item represents a file.
    IdRole,

    // This role returns boolean, which is a hint, if the tree node
    // should be expanded by default.
    ExpandedByDefaultRole,

    // This role returns source location (file name / line number).
    SourceLocationRole
};

// Maintainability of the code, 5 different levels. Excellent means that
// code is good, easy to maintain, appaling means, that code is hard to
// maintain.
enum Maintainability {
    Invalid = 0,
    Excellent = 1,
    Good = 2,
    Normal = 3,
    Bad = 4,
    Appalling = 5
};

} // namespace Constants
} // namespace CodeMetrics
