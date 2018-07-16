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

#include "generictextnote.h"


GenericTextNote::GenericTextNote(int column, QString text, QColor textColor, QBrush backgroundBrush, bool renderBackground, qreal cornerRadius, qreal margin)
    : m_column(column)
    , m_text(text)
    , m_textColor(textColor)
    , m_backgroundBrush(backgroundBrush)
    , m_renderBackground(renderBackground)
    , m_cornerRadius(cornerRadius)
    , m_margin(margin)
    , m_spaceLeft(false)
    , m_spaceRight(false)
{}

GenericTextNote::~GenericTextNote()
{}

int GenericTextNote::column() const
{
    return m_column;
}

qreal GenericTextNote::width(qreal height, const QFontMetricsF &fontMetrics) const
{
    qreal spaceWidth = fontMetrics.width(QChar::fromLatin1(' '));
    return fontMetrics.boundingRect(m_text).width() +
           m_margin * 2.0 +
           (m_spaceLeft ? spaceWidth : 0.0) +
           (m_spaceRight ? spaceWidth : 0.0);
}

void GenericTextNote::paint(qreal height, const QFontMetricsF &fontMetrics, const QFont &font, QPainter &painter) const
{
    qreal textWidth = width(height, fontMetrics);

    qreal spaceWidth = fontMetrics.width(QChar::fromLatin1(' '));
    qreal spaceMarginLeft  = (m_spaceLeft ? spaceWidth : 0.0);
    qreal spaceMarginRight = (m_spaceRight ? spaceWidth : 0.0);

    if (m_renderBackground) {
        QRectF rectangle(m_margin / 2.0 + spaceMarginLeft, 0, textWidth - m_margin - spaceMarginLeft - spaceMarginRight, height);
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_backgroundBrush);
        if (m_cornerRadius > 0) {
            painter.drawRoundedRect(rectangle, m_cornerRadius, m_cornerRadius);
        } else {
            painter.drawRect(rectangle);
        }
    }

    QPen pen(Qt::SolidLine);
    pen.setColor(m_textColor);
    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(m_margin + spaceMarginLeft, fontMetrics.ascent(), m_text);
}

void GenericTextNote::setText(QString text)
{
    m_text = text;
}

void GenericTextNote::setSpaceLeft(bool spaceLeft) {
    m_spaceLeft = spaceLeft;
}

void GenericTextNote::setSpaceRight(bool spaceRight) {
    m_spaceRight = spaceRight;
}
