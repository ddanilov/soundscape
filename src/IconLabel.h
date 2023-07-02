// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QLabel>

class IconLabel : public QLabel
{
public:
  explicit IconLabel(const QString& icon_file, QWidget* parent = nullptr);
};
