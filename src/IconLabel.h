// SPDX-FileCopyrightText: 2022-2024 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QLabel>

class IconLabel : public QLabel
{
  Q_OBJECT

public:
  explicit IconLabel(const QString& icon_file, QWidget* parent = nullptr);
};
