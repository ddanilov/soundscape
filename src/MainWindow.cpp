#include "MainWindow.h"

#include "JsonRW.h"
#include "Track.h"
#include "TrackControls.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_tray_available(QSystemTrayIcon::isSystemTrayAvailable()),
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    m_tray_icon(new QSystemTrayIcon(this)),
    m_tray_menu(new QMenu(this)),
#endif
    m_mouse_menu(new QMenu(this)),
    m_widget(new QWidget(this)),
    m_box_layout(new QVBoxLayout(m_widget)),
    m_menu_info(new QLabel(this))
{
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  setupTrayIcon();
#endif

  setWindowTitle("Soundscape");

  addPauseResumeItemsToMenu(m_mouse_menu);
  addTrackItemsToMenu(m_mouse_menu);
  addQuitItemToMenu(m_mouse_menu);

  setCentralWidget(m_widget);

  m_box_layout->setContentsMargins(0, 0, 0, 0);
  m_box_layout->setAlignment(Qt::AlignTop);

  m_menu_info->setTextFormat(Qt::PlainText);
  m_menu_info->setText("Use mouse right-click\n"
                       "to access application menu");
  m_box_layout->addWidget(m_menu_info, 0, Qt::AlignCenter);
}

void MainWindow::addPauseResumeItemsToMenu(QMenu* menu) const
{
  auto* pause_tracks = menu->addAction(tr("Pause playing tracks"));
  connect(pause_tracks, &QAction::triggered, this, &MainWindow::pausePlayingTracks);

  auto* resume_tracks = menu->addAction(tr("Resume paused tracks"));
  connect(resume_tracks, &QAction::triggered, this, &MainWindow::resumePausedTracks);
}

void MainWindow::addTrackItemsToMenu(QMenu* menu) const
{
  auto* add_track = menu->addAction(tr("Add track"));
  connect(add_track, &QAction::triggered, this, &MainWindow::addTrack);

  auto* save_track_list = menu->addAction(tr("Save track list"));
  connect(save_track_list, &QAction::triggered, this, &MainWindow::saveTrackList);

  auto* load_track_list = menu->addAction(tr("Load track list"));
  connect(load_track_list, &QAction::triggered, this, &MainWindow::loadTrackList);
}

void MainWindow::addQuitItemToMenu(QMenu* menu)
{
  menu->addSeparator();

  auto* quit_app = menu->addAction(tr("Quit"));
  connect(quit_app, &QAction::triggered, qApp, &QCoreApplication::quit);
}

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::trayIconAction(QSystemTrayIcon::ActivationReason reason)
{
  m_tray_icon->contextMenu()->exec(QCursor::pos());
}
#endif

void MainWindow::addTrack()
{
  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty())
  {
    addTrackFromMedia(file_name);
  }

  if (m_box_layout->count() > 1)
  {
    m_menu_info->hide();
  }
}

void MainWindow::saveTrackList()
{
  QString file_name = QFileDialog::getSaveFileName();
  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly))
    {
      qWarning("couldn't open file");
      return;
    }
    saveTracksToJson(file);
  }
}

void MainWindow::loadTrackList()
{
  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning("couldn't open file.");
      return;
    }
    loadTracksFromJson(file);
  }

  if (m_box_layout->count() > 1)
  {
    m_menu_info->hide();
  }
}

void MainWindow::moveTrackUp(const QString& id)
{
  auto* track = m_widget->findChild<TrackControls*>(id);
  if (track == nullptr) { return; }

  if (const auto new_index = m_box_layout->indexOf(track) - 1;
      new_index > 0) // menu info at index 0
  {
    m_box_layout->removeWidget(track);
    m_box_layout->insertWidget(new_index, track);
  }
}

void MainWindow::moveTrackDown(const QString& id)
{
  auto* track = m_widget->findChild<TrackControls*>(id);
  if (track == nullptr) { return; }

  if (const auto new_index = m_box_layout->indexOf(track) + 1;
      new_index < m_box_layout->count())
  {
    m_box_layout->removeWidget(track);
    m_box_layout->insertWidget(new_index, track);
  }
}

void MainWindow::removeTrack(const QString& id)
{
  auto* track = m_widget->findChild<TrackControls*>(id);
  if (track == nullptr) { return; }

  m_box_layout->removeWidget(track);
  track->deleteLater();

  if (m_box_layout->count() == 1)
  {
    m_menu_info->show();
  }
}

void MainWindow::pausePlayingTracks()
{
  for (auto* track_control : m_widget->findChildren<TrackControls*>())
  {
    track_control->pausePlaying();
  }
}

void MainWindow::resumePausedTracks()
{
  for (auto* track_control : m_widget->findChildren<TrackControls*>())
  {
    track_control->resumePaused();
  }
}

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::closeEvent(QCloseEvent* event)
{
  if (!m_tray_available) { event->accept(); }
  if (!m_tray_icon->isVisible()) { event->accept(); }

  windowHide();
  event->ignore();
}
#endif

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton)
  {
    m_mouse_menu->exec(QCursor::pos());
  }
}

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::setupTrayIcon()
{
  if (!m_tray_available) { return; }

  Qt::WindowFlags flags = Qt::CustomizeWindowHint |
                          Qt::WindowMaximizeButtonHint |
                          Qt::WindowCloseButtonHint;
  setWindowFlags(flags);

  auto* show_window = m_tray_menu->addAction(tr("Show window"));
  connect(show_window, &QAction::triggered, this, &MainWindow::windowShow);

  addPauseResumeItemsToMenu(m_tray_menu);
  addQuitItemToMenu(m_tray_menu);

  m_tray_icon->setContextMenu(m_tray_menu);
  connect(m_tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconAction);

  const QIcon& icon = QIcon(":/icons/icon.svg");
  m_tray_icon->setIcon(icon);
  setWindowIcon(icon);

  m_tray_icon->show();
}
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::windowFocus()
{
  raise();
  activateWindow();
}
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::windowShow()
{
  showNormal();
#if defined(Q_OS_LINUX)
  restoreGeometry(m_old_geometry);
#endif
  windowFocus();
}
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
void MainWindow::windowHide()
{
#if defined(Q_OS_LINUX)
  m_old_geometry = saveGeometry();
#endif
  showMinimized();
  setVisible(false);
}
#endif

void MainWindow::addTrackFromMedia(const QString& file_name)
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* track = new TrackControls(json, QDir(), this);
  m_box_layout->addWidget(track);
}

void MainWindow::saveTracksToJson(QFile& file)
{
  const QFileInfo file_info(file.fileName());
  const auto& base_dir = file_info.dir();
  QJsonObject data;
  QJsonArray tracks_data;
  for (int i = 0; i != m_box_layout->count(); ++i)
  {
    const auto* track_control = dynamic_cast<TrackControls*>(m_box_layout->itemAt(i)->widget());
    if (track_control) { tracks_data.append(track_control->track()->toJsonObject(base_dir)); }
  }
  data[JsonRW::TracksTag] = tracks_data;
  file.write(QJsonDocument(data).toJson());
}

void MainWindow::loadTracksFromJson(QFile& file)
{
  QFileInfo file_info(file.fileName());
  const auto& base_dir = file_info.dir();

  const auto& json_doc = QJsonDocument::fromJson(file.readAll());
  const auto& json = json_doc.object();

  if (json.contains(JsonRW::TracksTag) && json[JsonRW::TracksTag].isArray())
  {
    const auto& tracks = json[JsonRW::TracksTag].toArray();
    for (const auto& jdata : tracks)
    {
      auto* track = new TrackControls(jdata.toObject(), base_dir, this);
      m_box_layout->addWidget(track);
    }
  }
}
