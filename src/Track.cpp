#include "Track.h"

#include "JsonRW.h"

Track::Track(QObject* parent) :
    QObject(parent),
    m_file_name(),
    m_volume(0.5)
{
}

void Track::fromJsonObject(const QJsonObject& json, const QDir& base_dir)
{
  m_file_name = JsonRW::readString(JsonRW::FileNameTag, json).value_or(m_file_name);
  m_file_name = QDir::cleanPath(base_dir.absoluteFilePath(m_file_name));
  m_volume = JsonRW::readDouble(JsonRW::VolumeTag, json).value_or(m_volume);
}

QJsonObject Track::toJsonObject(const QDir& base_dir) const
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = base_dir.relativeFilePath(m_file_name);
  json[JsonRW::VolumeTag] = m_volume;
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
