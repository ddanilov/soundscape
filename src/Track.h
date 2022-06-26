#pragma once

#include <QDir>
#include <QObject>
#include <QPointer>

class Player;

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
  QPointer<Player> m_player;

  QString m_file_name;
  double m_volume;
  bool m_playing;

  friend class TestTrack;
};
