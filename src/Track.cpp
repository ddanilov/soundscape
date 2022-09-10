#include "Track.h"

#include "JsonRW.h"
#include "Player.h"

Track::Track(QObject* parent) :
    QObject(parent),
    m_player_A(new Player(this)),
    m_player_B(new Player(this)),
    m_file_name(),
    m_volume(0.5),
    m_playing(true),
    m_track_duration(-1),
    m_fade_in_duration(-1),
    m_fade_out_duration(-1),
    m_transition(Transition::FadeOutIn)
{
  m_player_A->setNextPlayer(m_player_B);
  m_player_B->setNextPlayer(m_player_A);

  connect(m_player_A, &Player::playerLoaded, this, &Track::playerLoaded);
  connect(m_player_A, &QMediaPlayer::errorOccurred, this, &Track::playerErrorOccurred);
  connect(m_player_B, &QMediaPlayer::errorOccurred, this, &Track::playerErrorOccurred);
}

void Track::fromJsonObject(const QJsonObject& json, const QDir& base_dir)
{
  m_file_name = JsonRW::readString(JsonRW::FileNameTag, json).value_or(m_file_name);
  if (!m_file_name.isEmpty())
  {
    m_file_name = QDir::cleanPath(base_dir.absoluteFilePath(m_file_name));
  }
  m_volume = JsonRW::readDouble(JsonRW::VolumeTag, json).value_or(m_volume);
  m_playing = JsonRW::readBool(JsonRW::PlayingTag, json).value_or(m_playing);
  m_fade_in_duration = JsonRW::readInteger(JsonRW::FadeInDurationTag, json).value_or(m_fade_in_duration);
  m_fade_out_duration = JsonRW::readInteger(JsonRW::FadeOutDurationTag, json).value_or(m_fade_out_duration);
  m_transition = static_cast<Transition>(JsonRW::readInteger(JsonRW::TransitionTag, json).value_or(static_cast<qint64>(m_transition)));

  const auto& source = QUrl::fromLocalFile(m_file_name);
  m_player_A->setSource(source);
}

QJsonObject Track::toJsonObject(const QDir& base_dir) const
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = base_dir.relativeFilePath(m_file_name);
  json[JsonRW::VolumeTag] = m_volume;
  json[JsonRW::PlayingTag] = m_playing;
  json[JsonRW::FadeInDurationTag] = m_fade_in_duration;
  json[JsonRW::FadeOutDurationTag] = m_fade_out_duration;
  json[JsonRW::TransitionTag] = static_cast<int>(m_transition);

  return json;
}

QString Track::title() const
{
  QFileInfo file_info(m_file_name);
  return file_info.baseName();
}

QString Track::fileName() const
{
  QFileInfo file_info(m_file_name);
  return file_info.fileName();
}

double Track::volume() const
{
  return m_volume;
}

void Track::setVolume(double volume)
{
  m_volume = volume;
}

float Track::fadeVolume(qint64 position) const
{
  const auto coeff = fade(position);
  auto volume = m_volume;
  switch (m_transition)
  {
    case Transition::FadeOutIn:
      volume = QAudio::convertVolume(coeff * volume, QAudio::VolumeScale::LogarithmicVolumeScale, QAudio::VolumeScale::LinearVolumeScale);
      break;
    case Transition::CrossFade:
      volume = coeff * QAudio::convertVolume(volume, QAudio::VolumeScale::LogarithmicVolumeScale, QAudio::VolumeScale::LinearVolumeScale);
      break;
  }
  return volume;
}

bool Track::isPlaying() const
{
  return m_playing;
}

void Track::play()
{
  m_playing = true;
  const auto a = m_player_A->playActive();
  const auto b = m_player_B->playActive();
  if (!(a || b)) { m_player_A->playActive(true); }
}

void Track::pause()
{
  m_playing = false;
  m_player_A->pauseActive();
  m_player_B->pauseActive();
}

qint64 Track::duration() const
{
  return m_track_duration;
}

qint64 Track::fadeInDuration() const
{
  return m_fade_in_duration;
}

void Track::setFadeInDuration(qint64 value)
{
  m_fade_in_duration = value;
}

qint64 Track::fadeOutDuration() const
{
  return m_fade_out_duration;
}

void Track::setFadeOutDuration(qint64 value)
{
  m_fade_out_duration = value;
}

Transition Track::transition() const
{
  return m_transition;
}

void Track::setTransition(Transition transition)
{
  m_transition = transition;
}

bool Track::startNextPlayer(qint64 position) const
{
  if (!m_playing) { return false; }
  auto threshold = m_track_duration;
  if (m_transition == Transition::CrossFade)
  {
    threshold -= m_fade_out_duration;
  }
  return position >= threshold;
}

Player* Track::playerA() const
{
  return m_player_A;
}

Player* Track::playerB() const
{
  return m_player_B;
}

const QList<QString>& Track::errors() const
{
  return m_errors;
}

void Track::playerLoaded()
{
  m_track_duration = m_player_A->duration();

  constexpr qint64 duration_5sec = 5 * 1000;
  if (m_fade_in_duration < 0) { m_fade_in_duration = std::min(duration_5sec, m_track_duration / 4); }
  if (m_fade_out_duration < 0) { m_fade_out_duration = m_fade_in_duration; }

  if (m_playing) { play(); }

  emit loaded();
}

void Track::playerErrorOccurred(QMediaPlayer::Error /*error*/, const QString& error_string)
{
  pause();
  m_errors.push_back(error_string);
  emit errorOccurred();
}

float Track::fade(qint64 position) const
{
  if (m_track_duration <= 0)
  {
    return 0.0;
  }

  if (position < 0 || position > m_track_duration)
  {
    return 0.0;
  }

  if (position < m_fade_in_duration)
  {
    return static_cast<float>(position) / static_cast<float>(m_fade_in_duration);
  }

  if (position > m_track_duration - m_fade_out_duration)
  {
    return static_cast<float>(m_track_duration - position) / static_cast<float>(m_fade_out_duration);
  }

  return 1.0;
}
