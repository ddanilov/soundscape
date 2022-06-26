#include "Track.h"

#include "JsonRW.h"
#include "Player.h"

Track::Track(QObject* parent) :
    QObject(parent),
    m_player(new Player(this)),
    m_file_name(),
    m_volume(0.5),
    m_playing(true)
{
}

void Track::fromJsonObject(const QJsonObject& json, const QDir& base_dir)
{
  m_file_name = JsonRW::readString(JsonRW::FileNameTag, json).value_or(m_file_name);
  m_file_name = QDir::cleanPath(base_dir.absoluteFilePath(m_file_name));
  m_volume = JsonRW::readDouble(JsonRW::VolumeTag, json).value_or(m_volume);
  m_playing = JsonRW::readBool(JsonRW::PlayingTag, json).value_or(m_playing);

  const auto& source = QUrl::fromLocalFile(m_file_name);
  m_player->setSource(source);
}

QJsonObject Track::toJsonObject(const QDir& base_dir) const
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = base_dir.relativeFilePath(m_file_name);
  json[JsonRW::VolumeTag] = m_volume;
  json[JsonRW::PlayingTag] = m_playing;
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
  m_player->setVolume(m_volume);
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
