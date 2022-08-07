#include "JsonRW.h"
#include "Player.h"
#include "Track.h"

#include <QSignalSpy>
#include <QTest>

class TestTrack : public QObject
{
  Q_OBJECT

public:
  explicit TestTrack(QObject* parent = nullptr) :
      QObject(parent),
      tmp_dir(),
      base_dir(tmp_dir.path()),
      file_name_audio_ok("./"),
      file_name_audio_duration_zero("./"),
      file_name_audio_broken("./"),
      file_name_media_without_audio("./")
  {}

private slots:
  void initTestCase();

  void testFromEmptyJson();
  void testFromJson();
  void testToJson();
  void testFade();
  void testAudioFileOk();
  void testAudioFileDurationZero();
  void testAudioFileBroken();
  void testMediaWithoutAudio();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;

  QString file_name_audio_ok;
  QString file_name_audio_duration_zero;
  QString file_name_audio_broken;
  QString file_name_media_without_audio;
};

void TestTrack::initTestCase()
{
  QFile media_file_ok(":/media/sound_0100.wav");
  file_name_audio_ok.append(QFileInfo(media_file_ok).fileName());
  file_name_audio_ok = QDir::cleanPath(base_dir.absoluteFilePath(file_name_audio_ok));
  QVERIFY(media_file_ok.copy(file_name_audio_ok));

  QFile media_file_duration_zero(":/media/sound_0000.wav");
  file_name_audio_duration_zero.append(QFileInfo(media_file_duration_zero).fileName());
  file_name_audio_duration_zero = QDir::cleanPath(base_dir.absoluteFilePath(file_name_audio_duration_zero));
  QVERIFY(media_file_duration_zero.copy(file_name_audio_duration_zero));

  QFile media_file_broken(":/media/sound_XXXX.wav");
  file_name_audio_broken.append(QFileInfo(media_file_broken).fileName());
  file_name_audio_broken = QDir::cleanPath(base_dir.absoluteFilePath(file_name_audio_broken));
  QVERIFY(media_file_broken.copy(file_name_audio_broken));

  QFile media_file_without_audio(":/media/video_0100.webm");
  file_name_media_without_audio.append(QFileInfo(media_file_without_audio).fileName());
  file_name_media_without_audio = QDir::cleanPath(base_dir.absoluteFilePath(file_name_media_without_audio));
  QVERIFY(media_file_without_audio.copy(file_name_media_without_audio));
}

void TestTrack::testFromEmptyJson()
{
  QJsonObject json;

  Track track;
  track.fromJsonObject(json, QDir());

  QCOMPARE(track.m_file_name, "");
  QCOMPARE(track.m_volume, 0.5);
  QCOMPARE(track.m_playing, true);
  QCOMPARE(track.m_track_duration, -1);
  QCOMPARE(track.m_fade_in_duration, -1);
  QCOMPARE(track.m_fade_out_duration, -1);

  QCOMPARE(track.title(), "");
  QCOMPARE(track.fileName(), "");
  QCOMPARE(track.volume(), 0.5);
  QCOMPARE(track.isPlaying(), true);
  QCOMPARE(track.duration(), -1);
  QCOMPARE(track.fadeInDuration(), -1);
  QCOMPARE(track.fadeOutDuration(), -1);
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
  track.fromJsonObject(json, base_dir);

  QCOMPARE(track.m_file_name, base_dir.absoluteFilePath("sound_01.mp3"));
  QCOMPARE(track.title(), "sound_01");
  QCOMPARE(track.fileName(), "sound_01.mp3");
  QCOMPARE(track.volume(), 0.37);
  QCOMPARE(track.isPlaying(), false);
  QCOMPARE(track.fadeInDuration(), 11'000);
  QCOMPARE(track.fadeOutDuration(), 13'000);
}

void TestTrack::testToJson()
{
  Track track;
  track.m_file_name = base_dir.absoluteFilePath("../sound_01.mp3");
  track.setVolume(0.15);
  track.m_playing = true;
  track.setFadeInDuration(123);
  track.setFadeOutDuration(456);

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

  track.setFadeInDuration(0);
  track.setFadeOutDuration(0);
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

  track.setFadeInDuration(10);
  track.setFadeOutDuration(20);
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

void TestTrack::testAudioFileOk()
{
  Track track;
  QSignalSpy loaded(&track, &Track::loaded);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_ok;
  track.fromJsonObject(json, base_dir);

  QVERIFY(loaded.wait());
  QVERIFY(track.errors().empty());

  QCOMPARE(track.duration(), 1000);
  QCOMPARE(track.fadeInDuration(), 250);
  QCOMPARE(track.fadeOutDuration(), 250);

  QCOMPARE(track.isPlaying(), true);
  track.pause();
  QCOMPARE(track.isPlaying(), false);
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);

  track.play();
  QCOMPARE(track.isPlaying(), true);
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PlayingState);
}

void TestTrack::testAudioFileDurationZero()
{
  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_duration_zero;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QVERIFY(!track.errors().front().isEmpty());
  QCOMPARE(track.duration(), -1);
}

void TestTrack::testAudioFileBroken()
{
  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_broken;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QVERIFY(!track.errors().front().isEmpty());
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);
}

void TestTrack::testMediaWithoutAudio()
{
  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_media_without_audio;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QCOMPARE(track.player()->playbackState(), QMediaPlayer::PausedState);
}

QTEST_MAIN(TestTrack)
#include "TestTrack.moc"
