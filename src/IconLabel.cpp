// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "IconLabel.h"

#include <QIcon>

IconLabel::IconLabel(const QString& icon_file, QWidget* parent) :
    QLabel(parent)
{
  const auto icon = QIcon(icon_file);
  const auto p = fontInfo().pixelSize();
  const auto h = static_cast<int>(1.5 * p);
  const auto w = 2 * h;
  setPixmap(icon.pixmap(w, h));
}
