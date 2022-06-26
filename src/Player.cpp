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
}

void Player::setVolume(double volume)
{
  const double vol = QAudio::convertVolume(volume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
  audioOutput()->setVolume(vol);
}

void Player::mediaPlayerStatusChanged(MediaStatus status)
{
  if (status == QMediaPlayer::MediaStatus::EndOfMedia)
  {
    pause();
    setPosition(0);
    play();
  }
}
