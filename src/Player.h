#pragma once

#include <QMediaPlayer>

class Track;

class Player : public QMediaPlayer
{
  Q_OBJECT

public:
  explicit Player(Track* parent);

  void setVolume(double volume);

private slots:
  void mediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);

private:
  const Track* m_track;

  friend class TestPlayer;
};
