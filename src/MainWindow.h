// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAtomicInteger>
#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

class TrackControls;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  void addPauseResumeItemsToMenu(QMenu* menu) const;
  void addTrackItemsToMenu(QMenu* menu) const;
  void addQuitItemToMenu(QMenu* menu) const;

public slots:
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  void trayIconAction(QSystemTrayIcon::ActivationReason reason);
#endif

  void addTrack();
  void saveTrackList();
  void loadTrackList();

  void loadExample(const QString& name);
  void loadExampleRain();
  void loadExampleRiver();

  void moveTrackUp(const QString& id);
  void moveTrackDown(const QString& id);
  void removeTrack(const QString& id);

  void pausePlayingTracks();
  void resumePausedTracks();

protected:
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  void closeEvent(QCloseEvent* event) override;
#endif
  void mousePressEvent(QMouseEvent* event) override;

private slots:
  void quit();

private:
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  void setupTrayIcon();
  void windowFocus();
  void windowShow();
  void windowHide();
#endif

  void addTrackFromMedia(const QString& file_name);
  void saveTracksToJson(QFile& file);
  void loadTracksFromJson(QFile& file);

  void showAbout();

  QAtomicInteger<bool> m_quit{false};
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
  bool m_tray_available;
  QPointer<QSystemTrayIcon> m_tray_icon;
  QPointer<QMenu> m_tray_menu;
#endif
  QPointer<QMenu> m_mouse_menu;
#if defined(Q_OS_LINUX)
  QByteArray m_old_geometry;
#endif

  QPointer<QWidget> m_widget;
  QPointer<QVBoxLayout> m_box_layout;
  QPointer<QLabel> m_menu_info;

  friend class TestMainWindow;
};
