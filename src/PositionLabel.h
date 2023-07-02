// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QLabel>

class PositionLabel : public QLabel
{
public:
  explicit PositionLabel(QWidget* parent = nullptr);
  void setMax(double value);
  void setValue(double value);

private:
  const QString m_text_template{"%1 s"};
  int m_field_width{3};
  int m_precision{1};

  friend class TestPositionLabel;
};
