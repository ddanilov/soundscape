#include "JsonRW.h"
#include "Player.h"
#include "Track.h"

#include <QSignalSpy>
#include <QTest>

class TestTrack : public QObject
{
  Q_OBJECT

public:
  TestTrack(QObject* parent = nullptr) :
      QObject(parent),
      tmp_dir(),
      base_dir(tmp_dir.path())
  {}

private slots:
  void testFromEmptyJson();
  void testFromJson();
  void testToJson();
  void testFade();
  void testPlayer();
  void testMediaFileOk();
  void testMediaFileBroken();
  void testMediaHasNoAudio();
  void testFadeDuration();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;
};

void TestTrack::testFromEmptyJson()
{
  QJsonObject json;

  Track track;
  track.fromJsonObject(json, QDir());

  QCOMPARE(track.m_file_name, "");
  QCOMPARE(track.m_volume, 0.5);
  QCOMPARE(track.m_playing, true);
  QCOMPARE(track.m_fade_in_duration, -1);
  QCOMPARE(track.m_fade_out_duration, -1);
}

void TestTrack::testFromJson()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = "sound_01.mp3";
  json[JsonRW::VolumeTag] = 0.37;
  json[JsonRW::PlayingTag] = false;
  json[JsonRW::FadeInDurationTag] = 11'000;
  json[JsonRW::FadeOutDurationTag] = 13'000;

  Track track;
  const auto& base_dir = QDir::temp();
  track.fromJsonObject(json, base_dir);

  QCOMPARE(track.m_file_name, base_dir.absoluteFilePath("sound_01.mp3"));
  QCOMPARE(track.m_volume, 0.37);
  QCOMPARE(track.m_playing, false);
  QCOMPARE(track.m_fade_in_duration, 11'000);
  QCOMPARE(track.m_fade_out_duration, 13'000);
}

void TestTrack::testToJson()
{
  const auto& base_dir = QDir::temp();

  Track track;
  track.m_file_name = base_dir.absoluteFilePath("../sound_01.mp3");
  track.m_volume = 0.15;
  track.m_playing = true;
  track.m_fade_in_duration = 123;
  track.m_fade_out_duration = 456;

  const QJsonObject& json = track.toJsonObject(base_dir);

  QVERIFY(json[JsonRW::FileNameTag].isString());
  QCOMPARE(json[JsonRW::FileNameTag].toString(), "../sound_01.mp3");

  QVERIFY(json[JsonRW::VolumeTag].isDouble());
  QCOMPARE(json[JsonRW::VolumeTag].toDouble(), 0.15);

  QVERIFY(json[JsonRW::PlayingTag].isBool());
  QCOMPARE(json[JsonRW::PlayingTag].toBool(), true);

  QVERIFY(json[JsonRW::FadeInDurationTag].isDouble());
  QCOMPARE(json[JsonRW::FadeInDurationTag].toInteger(), 123);

  QVERIFY(json[JsonRW::FadeOutDurationTag].isDouble());
  QCOMPARE(json[JsonRW::FadeOutDurationTag].toInteger(), 456);
}

void TestTrack::testFade()
{
  Track track;

  QCOMPARE(track.fade(-1), 0.0);
  QCOMPARE(track.fade(0), 0.0);
  QCOMPARE(track.fade(1), 0.0);

  track.m_track_duration = 100;
  QCOMPARE(track.fade(-1), 0.0);
  QCOMPARE(track.fade(0), 1.0);
  QCOMPARE(track.fade(1), 1.0);
  QCOMPARE(track.fade(50), 1.0);
  QCOMPARE(track.fade(99), 1.0);
  QCOMPARE(track.fade(100), 1.0);
  QCOMPARE(track.fade(101), 0.0);

  track.m_fade_in_duration = 0;
  track.m_fade_out_duration = 0;
  QCOMPARE(track.fade(-1), 0.0);
  QCOMPARE(track.fade(0), 1.0);
  QCOMPARE(track.fade(1), 1.0);
  QCOMPARE(track.fade(50), 1.0);
  QCOMPARE(track.fade(99), 1.0);
  QCOMPARE(track.fade(100), 1.0);
  QCOMPARE(track.fade(101), 0.0);

  auto compare_float_values = [](const float x, const float y, const float epsilon = 0.0001) {
    QVERIFY(std::abs(x - y) < epsilon);
  };

  track.m_fade_in_duration = 10;
  track.m_fade_out_duration = 20;
  QCOMPARE(track.fade(-1), 0.0);
  QCOMPARE(track.fade(0), 0.0);

  compare_float_values(track.fade(1), 0.1);
  compare_float_values(track.fade(5), 0.5);

  QCOMPARE(track.fade(10), 1.0);
  QCOMPARE(track.fade(50), 1.0);
  QCOMPARE(track.fade(80), 1.0);

  compare_float_values(track.fade(95), 0.25);
  compare_float_values(track.fade(99), 0.05);

  QCOMPARE(track.fade(100), 0.0);
  QCOMPARE(track.fade(101), 0.0);
}

void TestTrack::testPlayer()
{
  Track track;
  QVERIFY(track.isPlaying());

  QString file_name("sound_01.mp3");
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  const auto& base_dir = QDir::temp();
  track.fromJsonObject(json, base_dir);
  QCOMPARE(track.player()->source(), QUrl::fromLocalFile(base_dir.absoluteFilePath(file_name)));

  track.play();
  QCOMPARE(track.isPlaying(), true);
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PlayingState);

  track.pause();
  QCOMPARE(track.isPlaying(), false);
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);
}

void TestTrack::testMediaFileOk()
{
  QFile sound_file(":/media/sound_0100.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  Track track;
  QSignalSpy loaded(&track, &Track::loaded);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  track.fromJsonObject(json, base_dir);

  QVERIFY(loaded.wait());
  QVERIFY(track.errors().empty());

  QCOMPARE(track.duration(), 1000);
}

void TestTrack::testMediaFileBroken()
{
  QFile sound_file(":/media/sound_XXXX.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QVERIFY(!track.errors().front().isEmpty());
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);
}

void TestTrack::testMediaHasNoAudio()
{
  QFile media_file(":/media/picture_01.jpg");
  QString file_name("./");
  file_name.append(QFileInfo(media_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(media_file.copy(file_name));

  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);
}

void TestTrack::testFadeDuration()
{
  Track track;
  track.fromJsonObject(QJsonObject(), QDir());
  QCOMPARE(track.fadeInDuration(), -1);
  QCOMPARE(track.fadeOutDuration(), -1);

  track.setFadeInDuration(1234);
  track.setFadeOutDuration(6789);
  QCOMPARE(track.fadeInDuration(), 1234);
  QCOMPARE(track.fadeOutDuration(), 6789);
}

QTEST_MAIN(TestTrack)
#include "TestTrack.moc"
