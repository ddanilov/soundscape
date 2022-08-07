#include "Player.h"
#include "Track.h"

#include <QSignalSpy>
#include <QTest>

class TestPlayer : public QObject
{
  Q_OBJECT

public:
  explicit TestPlayer(QObject* parent = nullptr) :
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

  void testAudioFileOk();
  void testAudioFileDurationZero();
  void testAudioFileBroken();
  void testMediaFileWithoutAudio();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;

  QString file_name_audio_ok;
  QString file_name_audio_duration_zero;
  QString file_name_audio_broken;
  QString file_name_media_without_audio;
};

void TestPlayer::initTestCase()
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

void TestPlayer::testAudioFileOk()
{
  Track track;
  Player player(&track);
  QSignalSpy player_loaded(&player, &Player::playerLoaded);
  player.setSource(QUrl::fromLocalFile(file_name_audio_ok));
  QVERIFY(player_loaded.wait());
}

void TestPlayer::testAudioFileDurationZero()
{
  Track track;
  Player player(&track);
  QSignalSpy player_error(&player, &QMediaPlayer::errorOccurred);
  player.setSource(QUrl::fromLocalFile(file_name_audio_duration_zero));
  QVERIFY(player_error.wait());
  const auto& arguments = player_error.takeFirst();
  QCOMPARE(arguments.at(0).toInt(), QMediaPlayer::FormatError);
  QCOMPARE(arguments.at(1).toString(), "duration is 0");
}

void TestPlayer::testAudioFileBroken()
{
  Track track;
  Player player(&track);
  QSignalSpy player_error(&player, &QMediaPlayer::errorOccurred);
  player.setSource(QUrl::fromLocalFile(file_name_audio_broken));
  QVERIFY(player_error.wait());
}

void TestPlayer::testMediaFileWithoutAudio()
{
  Track track;
  Player player(&track);
  QSignalSpy player_error(&player, &QMediaPlayer::errorOccurred);
  player.setSource(QUrl::fromLocalFile(file_name_media_without_audio));
  QVERIFY(player_error.wait());
  const auto& arguments = player_error.takeFirst();
  QCOMPARE(arguments.at(0).toInt(), QMediaPlayer::FormatError);
  QCOMPARE(arguments.at(1).toString(), "track has no audio");
}

QTEST_MAIN(TestPlayer)
#include "TestPlayer.moc"
