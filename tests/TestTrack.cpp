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
  void testFadeVolume();
  void testAudioFileOk();
  void testAudioFileDurationZero();
  void testAudioFileBroken();
  void testMediaWithoutAudio();
  void testStartNextPlayer();
  void testStartDelay();

private:
  static void compare_float_values(const float x, const float y, const float epsilon = 0.001)
  {
    QVERIFY(std::abs(x - y) < epsilon);
  }

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
  QCOMPARE(track.m_transition, Transition::FadeOutIn);
  QCOMPARE(track.m_gap, 10);
  QCOMPARE(track.m_gap_max, 300);
  QCOMPARE(track.m_random_gap, false);

  QCOMPARE(track.title(), "");
  QCOMPARE(track.fileName(), "");
  QCOMPARE(track.volume(), 0.5);
  QCOMPARE(track.isPlaying(), true);
  QCOMPARE(track.duration(), -1);
  QCOMPARE(track.fadeInDuration(), -1);
  QCOMPARE(track.fadeOutDuration(), -1);
  QCOMPARE(track.transition(), Transition::FadeOutIn);
  QCOMPARE(track.gap(), 10);
  QCOMPARE(track.maxGap(), 300);
  QCOMPARE(track.randomGap(), false);
}

void TestTrack::testFromJson()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = "sound_01.mp3";
  json[JsonRW::VolumeTag] = 0.37;
  json[JsonRW::PlayingTag] = false;
  json[JsonRW::FadeInDurationTag] = 11'000;
  json[JsonRW::FadeOutDurationTag] = 13'000;
  json[JsonRW::TransitionTag] = static_cast<int>(Transition::CrossFade);
  json[JsonRW::GapTag] = 123;
  json[JsonRW::GapMaxTag] = 579;
  json[JsonRW::RandomGapTag] = true;

  Track track;
  track.fromJsonObject(json, base_dir);

  QCOMPARE(track.m_file_name, base_dir.absoluteFilePath("sound_01.mp3"));
  QCOMPARE(track.title(), "sound_01");
  QCOMPARE(track.fileName(), "sound_01.mp3");
  QCOMPARE(track.volume(), 0.37);
  QCOMPARE(track.isPlaying(), false);
  QCOMPARE(track.fadeInDuration(), 11'000);
  QCOMPARE(track.fadeOutDuration(), 13'000);
  QCOMPARE(track.transition(), Transition::CrossFade);
  QCOMPARE(track.gap(), 123);
  QCOMPARE(track.maxGap(), 579);
  QCOMPARE(track.randomGap(), true);
}

void TestTrack::testToJson()
{
  Track track;
  track.m_file_name = base_dir.absoluteFilePath("../sound_01.mp3");
  track.setVolume(0.15);
  track.m_playing = true;
  track.setFadeInDuration(123);
  track.setFadeOutDuration(456);
  track.setTransition(Transition::CrossFade);
  track.setGap(987);
  track.setMaxGap(13579);
  track.setRandomGap(true);

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

  QVERIFY(json[JsonRW::TransitionTag].isDouble());
  QCOMPARE(json[JsonRW::TransitionTag].toInteger(), static_cast<int>(Transition::CrossFade));

  QVERIFY(json[JsonRW::GapTag].isDouble());
  QCOMPARE(json[JsonRW::GapTag].toDouble(), 987);

  QVERIFY(json[JsonRW::GapMaxTag].isDouble());
  QCOMPARE(json[JsonRW::GapMaxTag].toDouble(), 13579);

  QVERIFY(json[JsonRW::RandomGapTag].isBool());
  QCOMPARE(json[JsonRW::RandomGapTag].toBool(), true);
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

void TestTrack::testFadeVolume()
{
  Track track;
  track.m_track_duration = 100;
  track.setFadeInDuration(10);
  track.setFadeOutDuration(20);
  track.m_volume = 1.0;

  {
    track.m_transition = Transition::FadeOutIn;
    QCOMPARE(track.fadeVolume(-1), 0.0);
    QCOMPARE(track.fadeVolume(0), 0.0);

    track.m_transition = Transition::FadeOutGapIn;
    QCOMPARE(track.fadeVolume(-1), 0.0);
    QCOMPARE(track.fadeVolume(0), 0.0);

    track.m_transition = Transition::CrossFade;
    QCOMPARE(track.fadeVolume(-1), 0.0);
    QCOMPARE(track.fadeVolume(0), 0.0);
  }

  {
    track.m_transition = Transition::FadeOutIn;
    compare_float_values(track.fadeVolume(track.fadeInDuration() / 2), 0.151);

    track.m_transition = Transition::FadeOutGapIn;
    compare_float_values(track.fadeVolume(track.fadeInDuration() / 2), 0.151);

    track.m_transition = Transition::CrossFade;
    compare_float_values(track.fadeVolume(track.fadeInDuration() / 2), 0.5);
  }

  {
    track.m_transition = Transition::FadeOutIn;
    compare_float_values(track.fadeVolume(track.fadeInDuration()), 1.0);

    track.m_transition = Transition::FadeOutGapIn;
    compare_float_values(track.fadeVolume(track.fadeInDuration()), 1.0);

    track.m_transition = Transition::CrossFade;
    compare_float_values(track.fadeVolume(track.fadeInDuration()), 1.0);
  }

  {
    track.m_transition = Transition::FadeOutIn;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration()), 1.0);

    track.m_transition = Transition::FadeOutGapIn;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration()), 1.0);

    track.m_transition = Transition::CrossFade;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration()), 1.0);
  }

  {
    track.m_transition = Transition::FadeOutIn;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration() / 2), 0.151);

    track.m_transition = Transition::FadeOutGapIn;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration() / 2), 0.151);

    track.m_transition = Transition::CrossFade;
    compare_float_values(track.fadeVolume(track.duration() - track.fadeOutDuration() / 2), 0.5);
  }

  {
    track.m_transition = Transition::FadeOutIn;
    QCOMPARE(track.fadeVolume(track.duration()), 0.0);
    QCOMPARE(track.fadeVolume(track.duration() + 1), 0.0);

    track.m_transition = Transition::FadeOutGapIn;
    QCOMPARE(track.fadeVolume(track.duration()), 0.0);
    QCOMPARE(track.fadeVolume(track.duration() + 1), 0.0);

    track.m_transition = Transition::CrossFade;
    QCOMPARE(track.fadeVolume(track.duration()), 0.0);
    QCOMPARE(track.fadeVolume(track.duration() + 1), 0.0);
  }
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
  QVERIFY(track.playerA()->playbackState() == QMediaPlayer::PlaybackState::PausedState);
  QVERIFY(track.playerB()->playbackState() == QMediaPlayer::PlaybackState::PausedState);

  track.play();
  QCOMPARE(track.isPlaying(), true);
  QVERIFY(track.playerA()->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(track.playerB()->playbackState() == QMediaPlayer::PlaybackState::PausedState);

  track.playerA()->mediaPlayerPositionChanged(track.duration());
  track.playerA()->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
  QVERIFY(track.playerA()->playbackState() == QMediaPlayer::PlaybackState::PausedState);
  QVERIFY(track.playerB()->playbackState() == QMediaPlayer::PlaybackState::PlayingState);

  track.pause();
  QVERIFY(track.playerA()->playbackState() == QMediaPlayer::PlaybackState::PausedState);
  QVERIFY(track.playerB()->playbackState() == QMediaPlayer::PlaybackState::PausedState);

  track.play();
  QVERIFY(track.playerA()->playbackState() == QMediaPlayer::PlaybackState::PausedState);
  QVERIFY(track.playerB()->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  QSKIP("Test does not work");
#endif

  Track track;
  QSignalSpy error(&track, &Track::errorOccurred);

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_broken;
  track.fromJsonObject(json, base_dir);

  QVERIFY(error.wait());
  QVERIFY(!track.errors().empty());
  QVERIFY(!track.errors().front().isEmpty());
  QVERIFY(track.playerA()->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
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
  QVERIFY(track.playerA()->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
}

void TestTrack::testStartNextPlayer()
{
  Track track;
  track.m_track_duration = 100;
  track.m_fade_in_duration = 10;
  track.m_fade_out_duration = 20;

  track.m_transition = Transition::FadeOutIn;
  QCOMPARE(track.startNextPlayer(-1), false);
  QCOMPARE(track.startNextPlayer(0), false);
  QCOMPARE(track.startNextPlayer(1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() - 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 0), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() + 0), true);
  QCOMPARE(track.startNextPlayer(track.duration() + 1), true);

  track.m_transition = Transition::FadeOutGapIn;
  QCOMPARE(track.startNextPlayer(-1), false);
  QCOMPARE(track.startNextPlayer(0), false);
  QCOMPARE(track.startNextPlayer(1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() - 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 0), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() + 0), true);
  QCOMPARE(track.startNextPlayer(track.duration() + 1), true);

  track.m_transition = Transition::CrossFade;
  QCOMPARE(track.startNextPlayer(-1), false);
  QCOMPARE(track.startNextPlayer(0), false);
  QCOMPARE(track.startNextPlayer(1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() - 1), false);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 0), true);
  QCOMPARE(track.startNextPlayer(track.duration() - track.fadeOutDuration() + 1), true);
  QCOMPARE(track.startNextPlayer(track.duration() - 1), true);
  QCOMPARE(track.startNextPlayer(track.duration() + 0), true);
  QCOMPARE(track.startNextPlayer(track.duration() + 1), true);
}

void TestTrack::testStartDelay()
{
  Track track;

  QCOMPARE(track.m_random_gap, false);
  QCOMPARE(track.startDelay(), track.m_gap * 1000);

  track.m_random_gap = true;
  const auto delay = track.startDelay();
  QVERIFY(delay >= track.m_gap * 1000);
  QVERIFY(delay < track.m_gap_max * 1000);
}

QTEST_MAIN(TestTrack)
#include "TestTrack.moc"
