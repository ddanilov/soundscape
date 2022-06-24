#include "Status.h"

#include <QFile>

Status::Status(QWidget* parent) :
    QCheckBox(parent)
{
  QFile file(":/styles/status.css");
  file.open(QIODevice::ReadOnly);
  const QString style = QString(file.readAll()).arg(":/icons/switch-on.svg", ":/icons/switch-off.svg");
  setStyleSheet(style);
}
