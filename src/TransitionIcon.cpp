#include "TransitionIcon.h"

#include <QDebug>
#include <QFile>

TransitionIcon::TransitionIcon(QWidget* parent) :
    QCheckBox(parent)
{
  QFile file(":/styles/transition-icon.css");
  file.open(QIODevice::ReadOnly);
  const QString style = QString(file.readAll()).arg(":/icons/fade-out-in.svg", ":/icons/cross-fade.svg", ":/icons/fade-gap.svg");
  setStyleSheet(style);
}
