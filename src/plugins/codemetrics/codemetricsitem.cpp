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

namespace CodeMetrics {
namespace Internal {

bool CodeMetricsItem::operator==(const CodeMetricsItem &other) const
{
    return file == other.file
            && lines == other.lines
            && linesOfCode == other.linesOfCode
            && linesOfComment == other.linesOfComment;
}

} // namespace Internal
} // namespace CodeMetrics
