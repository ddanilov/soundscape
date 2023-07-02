// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "PositionSlider.h"

PositionSlider::PositionSlider(QWidget* parent) :
    QSlider(parent)
{
  setOrientation(Qt::Horizontal);
  setRange(0, 500);
}
