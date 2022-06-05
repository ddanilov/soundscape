#include "MainWindow.h"

#include "TrackControls.h"

#include <QCloseEvent>
#include <QCoreApplication>
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

  auto* track = new TrackControls(this);
  m_box_layout->addWidget(track);

  raise();
  activateWindow();
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

void MainWindow::addItemsToMenu(QMenu* menu) const
{
  auto* add_track = menu->addAction(tr("Add track"));
  connect(add_track, &QAction::triggered, this, &MainWindow::addTrack);

  menu->addSeparator();

  auto* quit_app = menu->addAction(tr("Quit"));
  connect(quit_app, &QAction::triggered, qApp, &QCoreApplication::quit);
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
