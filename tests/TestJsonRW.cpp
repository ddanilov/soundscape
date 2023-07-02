// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

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
  void testReadInteger();
};

void TestJsonRW::testJsonTags()
{
  QCOMPARE(JsonRW::TracksTag, "tracks");
  QCOMPARE(JsonRW::FileNameTag, "fileName");
  QCOMPARE(JsonRW::VolumeTag, "volume");
  QCOMPARE(JsonRW::PlayingTag, "playing");
  QCOMPARE(JsonRW::FadeInDurationTag, "fadeInDuration");
  QCOMPARE(JsonRW::FadeOutDurationTag, "fadeOutDuration");
  QCOMPARE(JsonRW::TransitionTag, "transition");
  QCOMPARE(JsonRW::GapTag, "gap");
  QCOMPARE(JsonRW::GapMaxTag, "gapMax");
  QCOMPARE(JsonRW::RandomGapTag, "randomGap");
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

void TestJsonRW::testReadInteger()
{
  QJsonObject json;
  auto position = JsonRW::readInteger(JsonRW::FadeInDurationTag, json);
  QVERIFY(!position.has_value());

  const qint64 val = 123456789;
  json[JsonRW::FadeInDurationTag] = val;
  position = JsonRW::readDouble(JsonRW::FadeInDurationTag, json);
  QVERIFY(position.has_value());
  QCOMPARE(position.value(), val);
}

QTEST_MAIN(TestJsonRW)
#include "TestJsonRW.moc"
