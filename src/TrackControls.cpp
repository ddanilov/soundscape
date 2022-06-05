#include "TrackControls.h"

#include "MainWindow.h"

#include <QFile>
#include <QUuid>

TrackControls::TrackControls(MainWindow* parent) :
    QFrame(parent),
    m_layout(new QHBoxLayout(this))
{
  setObjectName(QUuid::createUuid().toString());

  auto spacing = fontInfo().pixelSize() / 2;
  QFile file(":/styles/track-controls.css");
  file.open(QIODevice::ReadOnly);
  const QString& style = QString(file.readAll()).arg(spacing).arg(spacing / 2);
  setStyleSheet(style);

  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->addWidget(new QLabel("Track: " + objectName()), 1, Qt::AlignLeft);
  setLayout(m_layout);
}
