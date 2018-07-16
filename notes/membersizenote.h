/*
 * Copyright 2018 Michal Srb <michalsrb@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef MEMBERSIZENOTE_H
#define MEMBERSIZENOTE_H

#include <QPen>
#include <QBrush>

#include <KTextEditor/InlineNoteInterface>


class MemberSizeNote : public KTextEditor::InlineNote
{
private:
    static constexpr uint64_t MAX_SQUARES = 16;

    static const QPen BORDER_PEN;
    static const QBrush EMPTY_BRUSH;
    static const QBrush FILLED_BRUSH;

public:
    MemberSizeNote(int column, uint64_t size, uint64_t padding, uint64_t offsetInParent, uint16_t byteGrouping = 0);
    ~MemberSizeNote() override = default;

    int column() const override;
    qreal width(qreal height, const QFontMetricsF& fontMetrics) const override;
    void paint(qreal height, const QFontMetricsF& fontMetrics, const QFont& font, QPainter& painter) const override;

    void setColumn(int column);

    uint64_t size() const;
    void setSize(uint64_t size);

    uint64_t padding() const;
    void setPadding(uint64_t padding);

private:
    int m_column;
    uint64_t m_size;
    uint64_t m_padding;
    uint64_t m_offsetInParent;
    uint16_t m_byteGrouping;
};

#endif // MEMBERSIZENOTE_H
