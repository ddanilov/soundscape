// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "Status.h"

#include <QFile>

Status::Status(QWidget* parent) :
    QCheckBox(parent)
{
  QFile file(":/styles/status.css");
  file.open(QIODevice::ReadOnly);
  QString style_template(file.readAll());
  m_playing_style = style_template.arg(":/icons/switch-on.svg", ":/icons/switch-off.svg");
  m_paused_style = style_template.arg(":/icons/switch-paused.svg", ":/icons/switch-off.svg");

  const auto p = fontInfo().pixelSize();
  const auto h = static_cast<int>(2.0 * p);
  const auto w = 2 * h;

  m_playing_style.append(QString("QCheckBox::indicator {width: %1; height: %2; }").arg(w).arg(h));
  m_paused_style.append(QString("QCheckBox::indicator {width: %1; height: %2; }").arg(w).arg(h));

  setPlayingStyle();

  updateToolTip(checkState());
  connect(this, &Status::stateChanged, this, &Status::updateToolTip);
}

void Status::setPlayingStyle()
{
  setStyleSheet(m_playing_style);
}

void Status::setPausedStyle()
{
  setStyleSheet(m_paused_style);
}

void Status::updateToolTip(int /*value*/)
{
  switch (checkState())
  {
    case Qt::CheckState::Unchecked:
      setToolTip(tr("paused"));
      break;
    case Qt::CheckState::Checked:
      setToolTip(tr("playing"));
      break;
  }
}
