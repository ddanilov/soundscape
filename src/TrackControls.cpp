#include "TrackControls.h"

#include "MainWindow.h"
#include "Track.h"

#include <QFile>
#include <QMouseEvent>
#include <QUuid>

TrackControls::TrackControls(const QJsonObject& json, const QDir& base_dir, MainWindow* parent) :
    QFrame(parent),
    m_main_window(parent),
    m_layout(new QHBoxLayout(this)),
    m_mouse_menu(new QMenu(this)),
    m_track(new Track(this))
{
  setObjectName(QUuid::createUuid().toString());

  auto spacing = fontInfo().pixelSize() / 2;
  QFile file(":/styles/track-controls.css");
  file.open(QIODevice::ReadOnly);
  const QString& style = QString(file.readAll()).arg(spacing).arg(spacing / 2);
  setStyleSheet(style);

  addItemsToMenu(m_mouse_menu);
  m_mouse_menu->addSeparator();
  m_main_window->addItemsToMenu(m_mouse_menu);

  setupControls();

  m_track->fromJsonObject(json, base_dir);

  updateControls();
}

void TrackControls::addItemsToMenu(QMenu* menu) const
{
  auto* move_track_up = menu->addAction(tr("Move Up"));
  connect(move_track_up, &QAction::triggered, this, &TrackControls::moveUp);

  auto* move_track_down = menu->addAction(tr("Move Down"));
  connect(move_track_down, &QAction::triggered, this, &TrackControls::moveDown);

  auto* remove_track = menu->addAction(tr("Remove"));
  connect(remove_track, &QAction::triggered, this, &TrackControls::remove);
}

void TrackControls::moveUp()
{
  m_main_window->moveTrackUp(objectName());
}

void TrackControls::moveDown()
{
  m_main_window->moveTrackDown(objectName());
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

void TrackControls::setupControls()
{
  m_layout->setContentsMargins(0, 0, 0, 0);
  setLayout(m_layout);
}

void TrackControls::updateControls()
{
  m_layout->addWidget(new QLabel(m_track->title()), 1, Qt::AlignLeft);
}
