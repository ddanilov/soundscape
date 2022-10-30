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
  void testNextPlayer();
  void testPlayPauseActive();

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
  auto* player = track.playerA();
  QVERIFY(!player->isReady());

  QSignalSpy player_loaded(player, &Player::playerLoaded);
  player->setSource(QUrl::fromLocalFile(file_name_audio_ok));
  QVERIFY(player_loaded.wait());
  QVERIFY(player->isReady());
}

void TestPlayer::testAudioFileDurationZero()
{
  Track track;
  auto* player = track.playerA();
  QSignalSpy player_error(player, &QMediaPlayer::errorOccurred);
  player->setSource(QUrl::fromLocalFile(file_name_audio_duration_zero));
  QVERIFY(player_error.wait());
  const auto& arguments = player_error.takeFirst();
  QCOMPARE(arguments.at(0).toInt(), QMediaPlayer::Error::FormatError);
  QCOMPARE(arguments.at(1).toString(), "duration is 0");
}

void TestPlayer::testAudioFileBroken()
{
  Track track;
  auto* player = track.playerA();
  QSignalSpy player_error(player, &QMediaPlayer::errorOccurred);
  player->setSource(QUrl::fromLocalFile(file_name_audio_broken));
  QVERIFY(player_error.wait());
}

void TestPlayer::testMediaFileWithoutAudio()
{
#if defined Q_OS_WIN
  QSKIP("Test does not work on Windows");
#endif

  Track track;
  auto* player = track.playerA();
  QSignalSpy player_error(player, &QMediaPlayer::errorOccurred);
  player->setSource(QUrl::fromLocalFile(file_name_media_without_audio));
  QVERIFY(player_error.wait());
  const auto& arguments = player_error.takeFirst();
  QCOMPARE(arguments.at(0).toInt(), QMediaPlayer::Error::FormatError);
  QCOMPARE(arguments.at(1).toString(), "track has no audio");
}

void TestPlayer::testNextPlayer()
{
  QPointer track = new Track;
  auto* player_A = track->playerA();
  auto* player_B = track->playerB();

  QCOMPARE(player_A->m_next_media_player, player_B);
  QCOMPARE(player_B->m_next_media_player, player_A);

  QSignalSpy player_A_loaded(player_A, &Player::playerLoaded);
  QSignalSpy player_B_loaded(player_B, &Player::playerLoaded);
  player_A->setSource(QUrl::fromLocalFile(file_name_audio_ok));
  QVERIFY(player_A_loaded.wait());
  QVERIFY(player_B_loaded.wait());
  QCOMPARE(player_B->source(), player_A->source());

  QSignalSpy player_A_state(player_A, &Player::playbackStateChanged);
  QSignalSpy player_B_state(player_B, &Player::playbackStateChanged);
  player_A->play();
  QVERIFY(player_A_state.wait());
  QVERIFY(player_B_state.wait());
  QVERIFY(player_A->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

  QCOMPARE(track->transition(), Transition::FadeOutIn);

  player_A->mediaPlayerPositionChanged(player_A->duration());
  player_A->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
  QVERIFY(player_A_state.wait());
  QVERIFY(player_B_state.wait());
  QVERIFY(player_A->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(player_B->playbackState() == QMediaPlayer::PlaybackState::PlayingState);

  player_B->mediaPlayerPositionChanged(player_B->duration());
  player_B->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
  QVERIFY(player_A_state.wait());
  QVERIFY(player_B_state.wait());
  QVERIFY(player_A->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

  track->setTransition(Transition::FadeOutGapIn);
  track->setGap(1.0);
  track->setRandomGap(false);

  const auto t1 = QDateTime::currentDateTime();
  player_A->mediaPlayerPositionChanged(player_A->duration());
  player_A->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
  QVERIFY(player_B_state.wait());
  QVERIFY(player_B->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  const auto t2 = QDateTime::currentDateTime();
  QVERIFY(t1.msecsTo(t2) >= track->gap() * 1000 * 0.95);
}

void TestPlayer::testPlayPauseActive()
{
  QPointer track = new Track;
  auto* player = track->playerA();
  QSignalSpy player_loaded(player, &Player::playerLoaded);
  player->setSource(QUrl::fromLocalFile(file_name_audio_ok));
  QVERIFY(player_loaded.wait());
  player->m_active = false;
  player->stop();

  QVERIFY(!player->playActive());
  QVERIFY(!player->m_active);
  QVERIFY(player->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

  QVERIFY(player->playActive(true));
  QVERIFY(player->m_active);
  QVERIFY(player->playbackState() == QMediaPlayer::PlaybackState::PlayingState);

  player->m_next_player_timer->setInterval(10'000);
  player->m_next_player_timer->start();
  QVERIFY(player->m_next_player_timer->isActive());
  player->pauseActive();
  QVERIFY(player->m_active);
  QVERIFY(player->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(!player->m_next_player_timer->isActive());

  QVERIFY(player->playActive());
  QVERIFY(player->m_active);
  QVERIFY(player->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
}

QTEST_MAIN(TestPlayer)
#include "TestPlayer.moc"
