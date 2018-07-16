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

#include "sourceinfotoolview.h"
#include "sourceinfoplugin.h"


SourceInfoToolView::SourceInfoToolView(QSharedPointer<SourceInfoConfig> config, QWidget* parent)
    : QWidget(parent)
    , m_config(config)
{
    Ui::SourceInfoToolView::setupUi(this);

    functionArgumentNamesCheck->setChecked(m_config->showFunctionArgumentNames);
    functionDefaultValuesCheck->setChecked(m_config->showFunctionArgumentDefaultValues);
    structFieldSizeCheck->setChecked(m_config->showStructFieldSize);
    autoTypeCheck->setChecked(m_config->showAutoType);
    enumValueCheck->setChecked(m_config->showEnumConstValues);

    connect(functionArgumentNamesCheck, &QCheckBox::stateChanged, this, &SourceInfoToolView::uiStateChanged);
    connect(functionDefaultValuesCheck, &QCheckBox::stateChanged, this, &SourceInfoToolView::uiStateChanged);
    connect(structFieldSizeCheck,       &QCheckBox::stateChanged, this, &SourceInfoToolView::uiStateChanged);
    connect(autoTypeCheck,              &QCheckBox::stateChanged, this, &SourceInfoToolView::uiStateChanged);
    connect(enumValueCheck,             &QCheckBox::stateChanged, this, &SourceInfoToolView::uiStateChanged);
}

SourceInfoToolView::~SourceInfoToolView()
{
}

void SourceInfoToolView::uiStateChanged()
{
    m_config->showFunctionArgumentNames = functionArgumentNamesCheck->isChecked();
    m_config->showFunctionArgumentDefaultValues = functionDefaultValuesCheck->isChecked();
    m_config->showStructFieldSize = structFieldSizeCheck->isChecked();
    m_config->showAutoType = autoTypeCheck->isChecked();
    m_config->showEnumConstValues = enumValueCheck->isChecked();

    emit m_config->changed();
}

void SourceInfoToolView::selectNextItem()
{
    // TODO ?
}

void SourceInfoToolView::selectPreviousItem()
{
    // TODO ?
}
