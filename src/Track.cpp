#include "Track.h"

#include "JsonRW.h"

Track::Track(QObject* parent) :
    QObject(parent)
{
}

void Track::fromJsonObject(const QJsonObject& json, const QDir& base_dir)
{
  m_file_name = JsonRW::readString(JsonRW::FileNameTag, json).value_or(m_file_name);
  m_file_name = QDir::cleanPath(base_dir.absoluteFilePath(m_file_name));
  qDebug() << m_file_name;
}

QJsonObject Track::toJsonObject(const QDir& base_dir) const
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = base_dir.relativeFilePath(m_file_name);
  return json;
}
