#include "JsonRW.h"

std::optional<QString> JsonRW::readString(const char* tag, const QJsonObject& json)
{
  const bool check = json.contains(tag) && json[tag].isString();
  if (check) { return json[tag].toString(); }
  return std::nullopt;
}
