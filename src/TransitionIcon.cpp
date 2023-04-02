#include "TransitionIcon.h"

#include "Transition.h"

#include <QDebug>
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

  qDebug() << w << h;
  style.append(QString("QCheckBox::indicator {width: %1; height: %2; }").arg(w).arg(h));
  qDebug() << style;
  setStyleSheet(style);

  updateToolTip(checkState());
  connect(this, &TransitionIcon::stateChanged, this, &TransitionIcon::updateToolTip);
}

void TransitionIcon::updateToolTip(int /*value*/)
{
  const auto transition = convertTransition(checkState());
  switch (transition)
  {
    case Transition::FadeOutIn:
      setToolTip("loop with fade-out/in");
      break;
    case Transition::CrossFade:
      setToolTip("loop with cross-fade");
      break;
    case Transition::FadeOutGapIn:
      setToolTip("loop with gap");
      break;
  }
}
