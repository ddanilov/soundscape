// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QCheckBox>

class Status : public QCheckBox
{
  Q_OBJECT

public:
  explicit Status(QWidget* parent = nullptr);
  void setPlayingStyle();
  void setPausedStyle();

private:
  void updateToolTip(int value);

  QString m_playing_style;
  QString m_paused_style;
};
