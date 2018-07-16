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

#ifndef SOURCEINFOTOOLVIEW_H
#define SOURCEINFOTOOLVIEW_H

#include <QWidget>

#include <interfaces/itoolviewactionlistener.h>

#include "sourceinfoinlinenoteprovider.h"
#include "ui_sourceinfotoolview.h"


class SourceInfoPlugin;


class SourceInfoToolView : public QWidget, Ui::SourceInfoToolView, public KDevelop::IToolViewActionListener
{
    Q_OBJECT
    Q_INTERFACES(KDevelop::IToolViewActionListener)

public:
    SourceInfoToolView(QSharedPointer<SourceInfoConfig> config, QWidget* parent);
    ~SourceInfoToolView() override;

public Q_SLOTS:
    void selectNextItem() override;
    void selectPreviousItem() override;

private Q_SLOTS:
    void uiStateChanged();

private:
    QSharedPointer<SourceInfoConfig> m_config;
};

#endif // SOURCEINFOTOOLVIEW_H
