#pragma once

#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPointer>

class MainWindow;
class Track;

class TrackControls : public QFrame
{
  Q_OBJECT

public:
  explicit TrackControls(const QJsonObject& json, const QDir& base_dir, MainWindow* parent = nullptr);
  void addItemsToMenu(QMenu* menu) const;

public slots:
  void moveUp();
  void moveDown();
  void remove();

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  MainWindow* m_main_window;

  QPointer<QHBoxLayout> m_layout;
  QPointer<QMenu> m_mouse_menu;

  QPointer<Track> m_track;
};
