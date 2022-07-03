#pragma once

#include <QMediaPlayer>

class Track;

class Player : public QMediaPlayer
{
  Q_OBJECT

public:
  explicit Player(Track* parent);

signals:
  void playerLoaded();

private slots:
  void mediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
  void mediaPlayerPositionChanged(qint64 position);

private:
  const Track* m_track;
};
