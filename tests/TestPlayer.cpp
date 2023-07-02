// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

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
  void testNextPlayerOutIn();
  void testNextPlayerOutGapIn();
  void testNextPlayerCrossFade();
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
}

void TestPlayer::testAudioFileBroken()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  QSKIP("Test does not work");
#endif

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
}

void TestPlayer::testNextPlayerOutIn()
{
  QPointer track = new Track;
  track->setTransition(Transition::FadeOutIn);

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

  auto test_transition = [&](auto* player_A, auto* player_B) {
    player_A->pauseActive();
    player_B->pauseActive();
    player_A->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
    QVERIFY(player_A->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  };

  for (int i = 0; i < 3; ++i)
  {
    test_transition(player_A, player_B);
    test_transition(player_B, player_A);
  }
}

void TestPlayer::testNextPlayerOutGapIn()
{
  QPointer track = new Track;
  track->setTransition(Transition::FadeOutGapIn);
  track->setGap(2.0);
  track->setRandomGap(false);

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

  auto test_transition = [&](auto* player_A, auto* player_B) {
    player_A->pauseActive();
    player_B->pauseActive();
    QSignalSpy player_B_state(player_B, &Player::playbackStateChanged);
    const auto t1_AB = QDateTime::currentDateTime();
    player_A->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
    QVERIFY(player_B_state.wait());
    QVERIFY(player_B->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
    const auto t2_AB = QDateTime::currentDateTime();
    QVERIFY(t1_AB.msecsTo(t2_AB) >= track->gap() * 1000 * 0.95);
  };

  for (int i = 0; i < 3; ++i)
  {
    test_transition(player_A, player_B);
    test_transition(player_B, player_A);
  }
}

void TestPlayer::testNextPlayerCrossFade()
{
  QPointer track = new Track;
  track->setTransition(Transition::CrossFade);
  track->setFadeInDuration(track->duration() / 4);
  track->setFadeOutDuration(track->duration() / 4);

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

  auto test_transition = [&](auto* player_A, auto* player_B) {
    player_A->pauseActive();
    player_B->pauseActive();
    player_A->mediaPlayerPositionChanged(player_A->duration() - track->fadeOutDuration() - 1);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
    player_A->pauseActive();
    player_B->pauseActive();
    player_A->mediaPlayerPositionChanged(player_A->duration() - track->fadeOutDuration() + 1);
    QVERIFY(player_B->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
  };

  for (int i = 0; i < 3; ++i)
  {
    test_transition(player_A, player_B);
    test_transition(player_B, player_A);
  }
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
