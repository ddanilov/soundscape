// SPDX-FileCopyrightText: 2022-2024 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "TransitionIcon.h"

#include "Transition.h"

#include <QFile>

TransitionIcon::TransitionIcon(QWidget* parent) :
    QCheckBox(parent)
{
  QFile file(":/styles/transition-icon.css");
  file.open(QIODevice::ReadOnly);
  QString style = QString(file.readAll()).arg(":/icons/fade-out-in.svg", ":/icons/cross-fade.svg", ":/icons/fade-gap.svg");

  const auto p = fontInfo().pixelSize();
  const auto h = static_cast<int>(2.0 * p);
  const auto w = 2 * h;

  style.append(QString("QCheckBox::indicator {width: %1; height: %2; }").arg(w).arg(h));
  setStyleSheet(style);

  updateToolTip(checkState());
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
  connect(this, &TransitionIcon::checkStateChanged, this, &TransitionIcon::updateToolTip);
#else
  connect(this, &TransitionIcon::stateChanged, this, &TransitionIcon::updateToolTip);
#endif
}

void TransitionIcon::updateToolTip(int /*value*/)
{
  const auto transition = convertTransition(checkState());
  switch (transition)
  {
    case Transition::FadeOutIn:
      setToolTip(tr("loop with fade-out/in"));
      break;
    case Transition::CrossFade:
      setToolTip(tr("loop with cross-fade"));
      break;
    case Transition::FadeOutGapIn:
      setToolTip(tr("loop with gap"));
      break;
  }
}
