#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QPointer>

class MainWindow;

class TrackControls : public QFrame
{
  Q_OBJECT

public:
  explicit TrackControls(MainWindow* parent = nullptr);

private:
  QPointer<QHBoxLayout> m_layout;
};
