#include "MainWindow.h"

#include "JsonRW.h"
#include "TrackControls.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QJsonObject>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_tray_icon(new QSystemTrayIcon(this)),
    m_tray_menu(new QMenu(this)),
    m_mouse_menu(new QMenu(this)),
    m_widget(new QWidget(this)),
    m_box_layout(new QVBoxLayout(m_widget)),
    m_menu_info(new QLabel(this))
{
  Qt::WindowFlags flags = Qt::CustomizeWindowHint |
                          Qt::WindowMaximizeButtonHint |
                          Qt::WindowCloseButtonHint;
  setWindowFlags(flags);

  setupTrayIcon();
  addItemsToMenu(m_tray_menu);
  addItemsToMenu(m_mouse_menu);

  setCentralWidget(m_widget);

  m_box_layout->setContentsMargins(0, 0, 0, 0);
  m_box_layout->setAlignment(Qt::AlignTop);

  m_menu_info->setTextFormat(Qt::PlainText);
  m_menu_info->setText("Use mouse right-click\n"
                       "to access application menu");
  m_box_layout->addWidget(m_menu_info, 0, Qt::AlignCenter);
}

void MainWindow::addItemsToMenu(QMenu* menu) const
{
  auto* add_track = menu->addAction(tr("Add track"));
  connect(add_track, &QAction::triggered, this, &MainWindow::addTrack);

  menu->addSeparator();

  auto* quit_app = menu->addAction(tr("Quit"));
  connect(quit_app, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::trayIconAction(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger)
  {
    windowShowOrHide();
  }
}

void MainWindow::addTrack()
{
  if (isMinimized())
  {
    windowShowOrHide();
  }

  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty())
  {
    QJsonObject json;
    json[JsonRW::FileNameTag] = file_name;
    auto* track = new TrackControls(json, QDir(), this);
    m_box_layout->addWidget(track);
  }

  if (m_box_layout->count() > 1)
  {
    m_menu_info->hide();
  }

  raise();
  activateWindow();
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

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (m_tray_icon->isVisible())
  {
    windowShowOrHide();
    event->ignore();
  }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::RightButton)
  {
    m_mouse_menu->exec(QCursor::pos());
  }
}

void MainWindow::setupTrayIcon()
{
  m_tray_icon->setContextMenu(m_tray_menu);
  connect(m_tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconAction);

  const QIcon& icon = QIcon(":/icons/icon.svg");
  m_tray_icon->setIcon(icon);
  setWindowIcon(icon);

  m_tray_icon->show();
}

void MainWindow::windowShowOrHide()
{
  if (isMinimized())
  {
    showNormal();
    restoreGeometry(m_old_geometry);
    raise();
    activateWindow();
    setFocus();
  }
  else
  {
    m_old_geometry = saveGeometry();
    showMinimized();
    setVisible(false);
  }
}
