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

#include <cassert>

#include <QPainter>

#include "membersizenote.h"


constexpr uint64_t MemberSizeNote::MAX_SQUARES;
const QPen MemberSizeNote::BORDER_PEN = QPen(Qt::black);
const QBrush MemberSizeNote::EMPTY_BRUSH = QBrush(Qt::white);
const QBrush MemberSizeNote::FILLED_BRUSH = QBrush(QColor(0xa0b0ff));


MemberSizeNote::MemberSizeNote(int column, uint64_t size, uint64_t padding, uint64_t offsetInParent, uint16_t byteGrouping)
    : m_column(column)
    , m_size(size)
    , m_padding(padding)
    , m_offsetInParent(offsetInParent)
    , m_byteGrouping(byteGrouping)
{
}

int MemberSizeNote::column() const
{
    return m_column;
}

qreal MemberSizeNote::width(qreal height, const QFontMetricsF& fontMetrics) const
{
    return height * (std::max(m_size, MAX_SQUARES) + std::max(m_padding, MAX_SQUARES));

    qreal x_offset = 0;
    int byte_counter = 0;
    auto measureRectangles = [&](uint64_t amount) {
        if (amount > MAX_SQUARES) {
            x_offset += height + 10;

            QString text = QString::number(amount) + "x";
            x_offset += fontMetrics.boundingRect(text).width();
        } else {
            for (int i = 0; i < amount; i++, byte_counter++) {
                x_offset += height;
                if (m_byteGrouping != 0 && (m_offsetInParent + byte_counter + 1) % m_byteGrouping == 0)
                    x_offset += height / 2;
            }
        }
    };

    measureRectangles(m_size);
    measureRectangles(m_padding);

    return x_offset;
}

void MemberSizeNote::paint(qreal height, const QFontMetricsF& fontMetrics, const QFont& font, QPainter& painter) const
{
    painter.setPen(BORDER_PEN);
    painter.setFont(font);

    qreal x_offset = 0;
    int byte_counter = 0;

    auto drawRectangles = [&](uint64_t amount) {
        if (amount > MAX_SQUARES) {
            painter.drawRect(x_offset + 1, 1, height - 3, height - 3);
            painter.drawRect(x_offset + 4, 1, height - 3, height - 3);
            painter.drawRect(x_offset + 7, 1, height - 3, height - 3);
            x_offset += height + 10;

            QString text = QString::number(amount) + "x";
            painter.drawText(x_offset, height - 3, text);
            x_offset += fontMetrics.boundingRect(text).width();

        } else {
            for (int i = 0; i < amount; i++, byte_counter++) {
                painter.drawRect(x_offset + 1, 1, height - 3, height - 3);
                x_offset += height;

                if (m_byteGrouping != 0 && (m_offsetInParent + byte_counter + 1) % m_byteGrouping == 0) {
                    x_offset += height / 2;
                }
            }
        }
    };

    painter.setBrush(FILLED_BRUSH);
    drawRectangles(m_size);

    painter.setBrush(EMPTY_BRUSH);
    drawRectangles(m_padding);
}

void MemberSizeNote::setColumn(int column)
{
    m_column = column;
}

uint64_t MemberSizeNote::size() const
{
    return m_size;
}

void MemberSizeNote::setSize(uint64_t size)
{
    m_size = size;
}

uint64_t MemberSizeNote::padding() const
{
    return m_padding;
}

void MemberSizeNote::setPadding(uint64_t padding)
{
    m_padding = padding;
}
