#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

struct JsonRW
{
  constexpr static const char* TracksTag = "tracks";
  constexpr static const char* FileNameTag = "fileName";
  constexpr static const char* VolumeTag = "volume";
  constexpr static const char* PlayingTag = "playing";
  constexpr static const char* FadeInDurationTag = "fadeInDuration";
  constexpr static const char* FadeOutDurationTag = "fadeOutDuration";
  constexpr static const char* TransitionTag = "transition";

  static std::optional<QString> readString(const char* tag, const QJsonObject& json);
  static std::optional<double> readDouble(const char* tag, const QJsonObject& json);
  static std::optional<bool> readBool(const char* tag, const QJsonObject& json);
  static std::optional<qint64> readInteger(const char* tag, const QJsonObject& json);
};
