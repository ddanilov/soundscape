#pragma once

#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  void addItemsToMenu(QMenu* menu) const;

public slots:
  void trayIconAction(QSystemTrayIcon::ActivationReason reason);

  void addTrack();
  void saveTrackList();
  void loadTrackList();

  void moveTrackUp(const QString& id);
  void moveTrackDown(const QString& id);
  void removeTrack(const QString& id);

protected:
  void closeEvent(QCloseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  void setupTrayIcon();
  void windowShowOrHide();

  void addTrackFromMedia(const QString& file_name);
  void saveTracksToJson(QFile& file);
  void loadTracksFromJson(QFile& file);

  QPointer<QSystemTrayIcon> m_tray_icon;
  QPointer<QMenu> m_tray_menu;
  QPointer<QMenu> m_mouse_menu;
  QByteArray m_old_geometry;

  QPointer<QWidget> m_widget;
  QPointer<QVBoxLayout> m_box_layout;
  QPointer<QLabel> m_menu_info;

  friend class TestMainWindow;
};
