#pragma once

#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPointer>

class MainWindow;
class Status;
class Track;
class Volume;

class TrackControls : public QFrame
{
  Q_OBJECT

public:
  explicit TrackControls(const QJsonObject& json, const QDir& base_dir, MainWindow* parent = nullptr);
  void addItemsToMenu(QMenu* menu) const;
  Track* track() const;

public slots:
  void moveUp();
  void moveDown();
  void remove();
  void volumeChanged(int value);

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  void setupControls();
  void updateControls();

  MainWindow* m_main_window;

  QPointer<QHBoxLayout> m_layout;
  QPointer<QMenu> m_mouse_menu;

  QPointer<Track> m_track;
  QPointer<Volume> m_volume_control;
  QPointer<Status> m_status_control;
};
