#include "JsonRW.h"

std::optional<QString> JsonRW::readString(const char* tag, const QJsonObject& json)
{
  const bool check = json.contains(tag) && json[tag].isString();
  if (check) { return json[tag].toString(); }
  return std::nullopt;
}

std::optional<double> JsonRW::readDouble(const char* tag, const QJsonObject& json)
{
  const bool check = json.contains(tag) && json[tag].isDouble();
  if (check) { return json[tag].toDouble(); }
  return std::nullopt;
}

std::optional<bool> JsonRW::readBool(const char* tag, const QJsonObject& json)
{
  const bool check = json.contains(tag) && json[tag].isBool();
  if (check) { return json[tag].toBool(); }
  return std::nullopt;
}
