#include "TrackControls.h"

#include "MainWindow.h"

#include <QFile>
#include <QMouseEvent>
#include <QUuid>

TrackControls::TrackControls(MainWindow* parent) :
    QFrame(parent),
    m_main_window(parent),
    m_layout(new QHBoxLayout(this)),
    m_mouse_menu(new QMenu(this))
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

  addItemsToMenu(m_mouse_menu);
  m_mouse_menu->addSeparator();
  m_main_window->addItemsToMenu(m_mouse_menu);
}

void TrackControls::addItemsToMenu(QMenu* menu) const
{
  auto* remove_track = menu->addAction(tr("Remove"));
  connect(remove_track, &QAction::triggered, this, &TrackControls::remove);
}

void TrackControls::remove()
{
  m_main_window->removeTrack(objectName());
}

void TrackControls::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton)
  {
    m_mouse_menu->exec(QCursor::pos());
  }
}
