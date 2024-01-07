// SPDX-FileCopyrightText: 2022-2024 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QSlider>

class PositionSlider : public QSlider
{
  Q_OBJECT

public:
  explicit PositionSlider(QWidget* parent = nullptr);
};
