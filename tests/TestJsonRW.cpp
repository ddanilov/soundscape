#include "JsonRW.h"

#include <QTest>

class TestJsonRW : public QObject
{
  Q_OBJECT

private slots:
  void testJsonTags();
  void testReadString();
  void testReadDouble();
  void testReadBool();
};

void TestJsonRW::testJsonTags()
{
  QCOMPARE(JsonRW::TracksTag, "tracks");
  QCOMPARE(JsonRW::FileNameTag, "fileName");
  QCOMPARE(JsonRW::VolumeTag, "volume");
}

void TestJsonRW::testReadString()
{
  QJsonObject json;
  auto name = JsonRW::readString(JsonRW::FileNameTag, json);
  QVERIFY(!name.has_value());

  const QString val("media_file.mp3");
  json[JsonRW::FileNameTag] = val;
  name = JsonRW::readString(JsonRW::FileNameTag, json);
  QVERIFY(name.has_value());
  QCOMPARE(name.value(), val);
}

void TestJsonRW::testReadDouble()
{
  QJsonObject json;
  auto volume = JsonRW::readDouble(JsonRW::VolumeTag, json);
  QVERIFY(!volume.has_value());

  const double val = 1.23;
  json[JsonRW::VolumeTag] = val;
  volume = JsonRW::readDouble(JsonRW::VolumeTag, json);
  QVERIFY(volume.has_value());
  QCOMPARE(volume.value(), val);
}

void TestJsonRW::testReadBool()
{
  QJsonObject json;
  auto playing = JsonRW::readBool(JsonRW::PlayingTag, json);
  QVERIFY(!playing.has_value());

  const bool val = false;
  json[JsonRW::PlayingTag] = val;
  playing = JsonRW::readBool(JsonRW::PlayingTag, json);
  QVERIFY(playing.has_value());
  QCOMPARE(playing.value(), val);
}

QTEST_MAIN(TestJsonRW)
#include "TestJsonRW.moc"
