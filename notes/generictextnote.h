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

#ifndef SAMPLEINLINETEXTNOTE_H
#define SAMPLEINLINETEXTNOTE_H

#include <QColor>
#include <QBrush>
#include <QFont>
#include <QPainter>

#include <KTextEditor/InlineNoteInterface>


class GenericTextNote : public KTextEditor::InlineNote
{
    constexpr static qreal MARGIN = 1.0;

public:
    GenericTextNote(int column, QString text, QColor textColor, QBrush backgroundBrush, bool renderBackground, qreal cornerRadius, qreal margin = MARGIN);
    virtual ~GenericTextNote();

    int column() const override;
    qreal width(qreal height, const QFontMetricsF &fontMetrics) const override;
    void paint(qreal height, const QFontMetricsF &fontMetrics, const QFont &font, QPainter &painter) const override;

    void setText(QString text);

    void setSpaceLeft(bool spaceLeft);
    void setSpaceRight(bool spaceRight);

private:
    int m_column;
    QString m_text;

    QColor m_textColor;
    QBrush m_backgroundBrush;
    bool m_renderBackground;
    qreal m_cornerRadius;

    qreal m_margin;

    bool m_spaceLeft;
    bool m_spaceRight;
};

#endif // SAMPLEINLINETEXTNOTE_H
