#pragma once

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
  static void addQuitItemToMenu(QMenu* menu);

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

  bool m_tray_available;
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
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
