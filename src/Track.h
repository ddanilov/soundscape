#pragma once

#include <QDir>
#include <QObject>

class Track : public QObject
{
  Q_OBJECT

public:
  explicit Track(QObject* parent = nullptr);

  void fromJsonObject(const QJsonObject& json, const QDir& base_dir);
  QJsonObject toJsonObject(const QDir& base_dir) const;

  QString title() const;

  double volume() const;
  void setVolume(double volume);

  bool isPlaying() const;
  void play();
  void pause();

private:
  QString m_file_name;
  double m_volume;
  bool m_playing;
};
