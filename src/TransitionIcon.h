// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QCheckBox>

class TransitionIcon : public QCheckBox
{
public:
  explicit TransitionIcon(QWidget* parent = nullptr);

private:
  void updateToolTip(int value);
};
