#include "Player.h"

#include "Track.h"

#include <QAudioOutput>

Player::Player(Track* parent) :
    QMediaPlayer(parent),
    m_track(parent)
{
  setAudioOutput(new QAudioOutput(this));
  audioOutput()->setVolume(0);

  connect(this, &QMediaPlayer::mediaStatusChanged, this, &Player::mediaPlayerStatusChanged);
  connect(this, &QMediaPlayer::positionChanged, this, &Player::mediaPlayerPositionChanged);
}

void Player::mediaPlayerStatusChanged(MediaStatus status)
{
  if (status == QMediaPlayer::MediaStatus::LoadedMedia)
  {
    if (duration() <= 0)
    {
      stop();
      emit errorOccurred(QMediaPlayer::FormatError, QString("duration is 0"));
    }
    emit playerLoaded();
  }

  if (status == QMediaPlayer::MediaStatus::EndOfMedia)
  {
    pause();
    setPosition(0);
    play();
  }
}

void Player::mediaPlayerPositionChanged(qint64 position)
{
  const auto volume = m_track->fadeVolume(position);
  audioOutput()->setVolume(volume);
}
