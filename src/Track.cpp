#include "Track.h"

#include "JsonRW.h"
#include "Player.h"

Track::Track(QObject* parent) :
    QObject(parent),
    m_player(new Player(this)),
    m_file_name(),
    m_volume(0.5),
    m_playing(true),
    m_track_duration(-1),
    m_fade_in_duration(-1),
    m_fade_out_duration(-1)
{
  connect(m_player, &Player::playerLoaded, this, &Track::playerLoaded);
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

  const auto& source = QUrl::fromLocalFile(m_file_name);
  m_player->setSource(source);
}

QJsonObject Track::toJsonObject(const QDir& base_dir) const
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = base_dir.relativeFilePath(m_file_name);
  json[JsonRW::VolumeTag] = m_volume;
  json[JsonRW::PlayingTag] = m_playing;
  json[JsonRW::FadeInDurationTag] = m_fade_in_duration;
  json[JsonRW::FadeOutDurationTag] = m_fade_out_duration;

  return json;
}

QString Track::title() const
{
  QFileInfo fileInfo(m_file_name);
  return fileInfo.baseName();
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
  return QAudio::convertVolume(fade(position) * m_volume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
}

bool Track::isPlaying() const
{
  return m_playing;
}

void Track::play()
{
  m_playing = true;
  m_player->play();
}

void Track::pause()
{
  m_playing = false;
  m_player->pause();
}

void Track::playerLoaded()
{
  m_track_duration = m_player->duration();

  constexpr qint64 duration_5sec = 5 * 1000;
  if (m_fade_in_duration < 0) { m_fade_in_duration = std::min(duration_5sec, m_track_duration / 4); }
  if (m_fade_out_duration < 0) { m_fade_out_duration = m_fade_in_duration; }
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
