#pragma once

#include <QMediaPlayer>
#include <QPointer>
#include <QTimer>

class Track;

class Player : public QMediaPlayer
{
  Q_OBJECT

public:
  explicit Player(Track* parent);
  bool isReady() const;
  void setNextPlayer(Player* mediaPlayer);
  bool playActive(bool force = false);
  void pauseActive();

signals:
  void playerLoaded();

private slots:
  void mediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
  void mediaPlayerPositionChanged(qint64 position);
  void setupNextPlayer();

private:
  void startNextPlayer(qint64 position);

  const Track* m_track;

  bool m_ready;
  bool m_active;
  bool m_next_media_player_started;
  Player* m_next_media_player;
  QPointer<QTimer> m_next_player_timer;

  friend class TestPlayer;
  friend class TestTrack;
  friend class TestTrackControls;
};
