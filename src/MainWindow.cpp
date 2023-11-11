// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "MainWindow.h"

#include "JsonRW.h"
#include "Track.h"
#include "TrackControls.h"
#include "Version.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(const bool disable_tray, QWidget* parent) :
    QMainWindow(parent),
    m_tray_available(disable_tray ? false : QSystemTrayIcon::isSystemTrayAvailable()),
    m_tray_icon(new QSystemTrayIcon(this)),
    m_tray_menu(new QMenu(this)),
    m_mouse_menu(new QMenu(this)),
    m_widget(new QWidget(this)),
    m_box_layout(new QVBoxLayout(m_widget)),
    m_menu_info(new QLabel(this))
{
  setupTrayIcon();

  setWindowTitle(APP_TITLE);

  addPauseResumeItemsToMenu(m_mouse_menu);
  addTrackItemsToMenu(m_mouse_menu);
  addQuitItemToMenu(m_mouse_menu);

  setCentralWidget(m_widget);

  m_box_layout->setContentsMargins(0, 0, 0, 0);
  m_box_layout->setAlignment(Qt::AlignTop);

  m_menu_info->setTextFormat(Qt::PlainText);
  m_menu_info->setText(tr("Use mouse right-click\n"
                          "to access application menu"));
  m_box_layout->addWidget(m_menu_info, 0, Qt::AlignCenter);

#if defined(Q_OS_MACOS)
  auto* menu_bar = new QMenuBar(nullptr);
  auto* about_menu = menu_bar->addMenu(tr("About"));
  auto* about_action = about_menu->addAction(tr("About"));
  connect(about_action, &QAction::triggered, this, &MainWindow::showAbout);
#endif
}

void MainWindow::start(const QString& file_name, const bool hidden)
{
  if (m_tray_available && hidden)
  {
    hide();
  }
  else
  {
    show();
  }

  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    loadTracksFromJson(file);
  }
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

  auto* example_menu = new QMenu(tr("Examples"));
  menu->addMenu(example_menu);

  auto* example_rain = example_menu->addAction(tr("Rain and Thunder"));
  connect(example_rain, &QAction::triggered, this, &MainWindow::loadExampleRain);

  auto* example_river = example_menu->addAction(tr("River"));
  connect(example_river, &QAction::triggered, this, &MainWindow::loadExampleRiver);
}

void MainWindow::addQuitItemToMenu(QMenu* menu) const
{
  menu->addSeparator();

#if !defined(Q_OS_MACOS)
  auto* about_app = menu->addAction(tr("About"));
  connect(about_app, &QAction::triggered, this, &MainWindow::showAbout);
#endif

  auto* quit_app = menu->addAction(tr("Quit"));
  connect(quit_app, &QAction::triggered, this, &MainWindow::quit);
}

void MainWindow::trayIconAction(QSystemTrayIcon::ActivationReason /*reason*/)
{
  m_tray_icon->contextMenu()->exec(QCursor::pos());
}

void MainWindow::addTrack()
{
  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty())
  {
    addTrackFromMedia(file_name);
  }
}

void MainWindow::saveTrackList()
{
  QString file_name = QFileDialog::getSaveFileName();
  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    saveTracksToJson(file);
  }
}

void MainWindow::loadTrackList()
{
  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty())
  {
    QFile file(file_name);
    loadTracksFromJson(file);
  }
}

void MainWindow::loadExample(const QString& name)
{
  const auto& app_dir = QDir(QCoreApplication::applicationDirPath());
  QString file_name("../");
#if defined Q_OS_MACOS
  file_name.append("Resources");
#else
  file_name.append(CMAKE_INSTALL_DATADIR);
#endif
  file_name.append("/examples/");
  file_name.append(name);
  file_name = QDir::cleanPath(app_dir.absoluteFilePath(file_name));
  QFile file(file_name);
  loadTracksFromJson(file);
}

void MainWindow::loadExampleRain()
{
  loadExample("rain_and_thunder/rain_and_thunder.json");
}

void MainWindow::loadExampleRiver()
{
  loadExample("river/river.json");
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

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (m_quit || !m_tray_available)
  {
    event->accept();
    return;
  }

  windowHide();
  event->ignore();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton)
  {
    m_mouse_menu->exec(QCursor::pos());
  }
}

void MainWindow::quit()
{
  m_quit = true;
  QApplication::quit();
}

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

void MainWindow::windowFocus()
{
  raise();
  activateWindow();
}

void MainWindow::windowShow()
{
  showNormal();
#if defined(Q_OS_LINUX)
  restoreGeometry(m_old_geometry);
#endif
  windowFocus();
}

void MainWindow::windowHide()
{
#if defined(Q_OS_LINUX)
  m_old_geometry = saveGeometry();
#endif
  showMinimized();
  setVisible(false);
}

void MainWindow::addTrackFromMedia(const QString& file_name)
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* track = new TrackControls(json, QDir(), this);
  m_box_layout->addWidget(track);
  m_menu_info->hide();
}

void MainWindow::saveTracksToJson(QFile& file)
{
  if (!file.open(QIODevice::WriteOnly))
  {
    const auto& title = tr("Save Tracks");
    const auto& message = tr("couldn't open file: %1").arg(file.fileName());
    QMessageBox::warning(this, title, message);
    return;
  }

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
  if (!file.open(QIODevice::ReadOnly))
  {
    const auto& title = tr("Load Tracks");
    const auto& message = tr("couldn't open file: %1").arg(file.fileName());
    QMessageBox::warning(this, title, message);
    return;
  }

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

  if (m_box_layout->count() > 1)
  {
    m_menu_info->hide();
  }
}

void MainWindow::showAbout()
{
  QString info;
  info.append(QString("<h3>%1</h3>\n").arg(APP_TITLE));
  info.append("<h4>" + tr("Version: %1").arg(APP_VERSION) + "</h4>" + "\n");

  info.append(tr("open-source system-tray resident desktop application for playing soundscapes") + "<br>" + "\n");
  info.append("<br>");

  info.append(tr(R"(Website: <a href="%1">%1</a>)").arg(WEB_SITE) + "<br>" + "\n");
  info.append(tr("Copyright: © 2022-2023 Denis Danilov") + "<br>" + "\n");
  info.append(tr("License: GNU General Public License (GPL) version 3") + "<br>" + "\n");
  info.append("<br>");

  info.append(tr("Qt Version: %1").arg(qVersion()) + "\n");
  info.append("<br>");

  QMessageBox::about(this, tr("About"), info);
}
