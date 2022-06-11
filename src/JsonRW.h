#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

struct JsonRW
{
  constexpr static const char* TracksTag = "tracks";
  constexpr static const char* FileNameTag = "fileName";
  constexpr static const char* VolumeTag = "volume";

  static std::optional<QString> readString(const char* tag, const QJsonObject& json);
  static std::optional<double> readDouble(const char* tag, const QJsonObject& json);
};
