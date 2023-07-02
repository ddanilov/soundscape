// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QSlider>

class PositionSlider : public QSlider
{
public:
  explicit PositionSlider(QWidget* parent = nullptr);
};
