// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "Player.h"

#include "Track.h"

#include <QAudioOutput>

Player::Player(Track* parent) :
    QMediaPlayer(parent),
    m_track(parent),
    m_ready(false),
    m_active(false),
    m_next_media_player(nullptr),
    m_next_player_timer(new QTimer(this))
{
  setAudioOutput(new QAudioOutput(this));
  audioOutput()->setVolume(0);

  connect(this, &QMediaPlayer::mediaStatusChanged, this, &Player::mediaPlayerStatusChanged);
  connect(this, &QMediaPlayer::positionChanged, this, &Player::mediaPlayerPositionChanged);

  m_next_player_timer->setSingleShot(true);
}

bool Player::isReady() const
{
  return m_ready;
}

void Player::setNextPlayer(Player* mediaPlayer)
{
  m_next_media_player = mediaPlayer;
}

bool Player::playActive(const bool force)
{
  if (force) { m_active = true; }
  if (m_active)
  {
    play();
    return true;
  }
  return false;
}

void Player::pauseActive()
{
  m_next_player_timer->stop();
  pause();
}

void Player::mediaPlayerStatusChanged(MediaStatus status)
{
  if (!m_ready && status == QMediaPlayer::MediaStatus::LoadedMedia)
  {
    if (!hasAudio())
    {
      emit errorOccurred(QMediaPlayer::Error::FormatError, QString("track has no audio"));
      return;
    }

    if (duration() <= 0)
    {
      emit errorOccurred(QMediaPlayer::Error::FormatError, QString("duration is 0"));
      return;
    }
    m_ready = true;
    emit playerLoaded();
    setupNextPlayer();
    return;
  }

  if (m_ready && status == QMediaPlayer::MediaStatus::EndOfMedia)
  {
    m_active = false;
    if (m_track->transition() == Transition::FadeOutIn ||
        m_track->transition() == Transition::FadeOutGapIn)
    {
      startNextPlayerOutIn();
    }
    pause();
    setPosition(0);
  }
}

void Player::mediaPlayerPositionChanged(qint64 position)
{
  if (!m_active) { return; }
  const auto volume = m_track->fadeVolume(position);
  audioOutput()->setVolume(volume);
  startNextPlayerCrossFade(position);
}

void Player::setupNextPlayer()
{
  if (m_next_media_player)
  {
    connect(m_next_player_timer, &QTimer::timeout, m_next_media_player, [this]() { m_next_media_player->playActive(true); });
    if (!m_next_media_player->isReady()) { m_next_media_player->setSource(source()); }
  }
}

void Player::startNextPlayerOutIn()
{
  if (m_next_media_player->playbackState() == QMediaPlayer::PlaybackState::PlayingState) { return; }
  if (m_track->transition() == Transition::FadeOutGapIn)
  {
    m_next_player_timer->stop();
    const auto delay = m_track->startDelay();
    m_next_player_timer->setInterval(delay);
    m_next_player_timer->start();
  }
  else
  {
    m_next_media_player->playActive(true);
  }
}

void Player::startNextPlayerCrossFade(qint64 position)
{
  if (m_track->transition() != Transition::CrossFade) { return; }
  if (m_next_media_player->playbackState() == QMediaPlayer::PlaybackState::PlayingState) { return; }
  if (m_track->startNextPlayer(position))
  {
    m_next_media_player->playActive(true);
  }
}
