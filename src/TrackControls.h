// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "Transition.h"

#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QMenu>
#include <QPointer>

class MainWindow;
class Status;
class Track;
class TrackSettings;
class TransitionIcon;
class Volume;

class TrackControls : public QFrame
{
  Q_OBJECT

public:
  explicit TrackControls(const QJsonObject& json, const QDir& base_dir, MainWindow* parent);
  Track* track() const;
  void pausePlaying();
  void resumePaused();

public slots:
  void moveUp();
  void moveDown();
  void remove();
  void volumeChanged(int value);
  void transitionChanged(int state);
  void statusChanged(int state);
  void trackLoaded();
  void playerError();

signals:
  void updated();

protected:
  void mousePressEvent(QMouseEvent* event) override;
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  void addItemsToMenu(QMenu* menu) const;
  void setupControls();
  void disableControls();
  void enableControls();
  void updateControls();

  MainWindow* m_main_window;

  QPointer<QHBoxLayout> m_layout;
  QPointer<QMenu> m_mouse_menu;

  QPointer<Track> m_track;
  QPointer<Volume> m_volume_control;
  QPointer<TransitionIcon> m_transition_control;
  QPointer<Status> m_status_control;

  QPointer<TrackSettings> m_settings;

  friend class TestTrackControls;
};
