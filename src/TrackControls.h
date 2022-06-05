#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPointer>

class MainWindow;

class TrackControls : public QFrame
{
  Q_OBJECT

public:
  explicit TrackControls(MainWindow* parent = nullptr);
  void addItemsToMenu(QMenu* menu) const;

public slots:
  void remove();

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  MainWindow* m_main_window;

  QPointer<QHBoxLayout> m_layout;
  QPointer<QMenu> m_mouse_menu;
};
