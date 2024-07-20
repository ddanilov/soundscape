// SPDX-FileCopyrightText: 2022-2024 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "JsonRW.h"
#include "MainWindow.h"
#include "Player.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"
#include "TransitionIcon.h"
#include "Volume.h"

#include <QSignalSpy>
#include <QTest>

class TestTrackControls : public QObject
{
  Q_OBJECT

public:
  explicit TestTrackControls(QObject* parent = nullptr) :
      QObject(parent),
      tmp_dir(),
      base_dir(tmp_dir.path()),
      file_name_audio_ok("./"),
      file_name_audio_broken("./")
  {}

private slots:
  void initTestCase();

  void testAudioFileOk();
  void testAudioFileBroken();
  void testMenu();
  void testPauseAndResume();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;

  QString file_name_audio_ok;
  QString file_name_audio_broken;
};

void TestTrackControls::initTestCase()
{
  QFile media_file_ok(":/media/sound_0100.wav");
  file_name_audio_ok.append(QFileInfo(media_file_ok).fileName());
  file_name_audio_ok = QDir::cleanPath(base_dir.absoluteFilePath(file_name_audio_ok));
  QVERIFY(media_file_ok.copy(file_name_audio_ok));

  QFile media_file_broken(":/media/sound_XXXX.wav");
  file_name_audio_broken.append(QFileInfo(media_file_broken).fileName());
  file_name_audio_broken = QDir::cleanPath(base_dir.absoluteFilePath(file_name_audio_broken));
  QVERIFY(media_file_broken.copy(file_name_audio_broken));
}

void TestTrackControls::testAudioFileOk()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_ok;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto volume_control = track_controls->m_volume_control;
  QCOMPARE(volume_control->value(), 50);

  auto transition_control = track_controls->m_transition_control;
  QCOMPARE(transition_control->isEnabled(), true);
  QCOMPARE(transition_control->checkState(), Qt::CheckState::Unchecked);
  QCOMPARE(track->transition(), Transition::FadeOutIn);

  // QTest::mouseClick(transition_control, Qt::MouseButton::LeftButton);
  transition_control->setCheckState(Qt::CheckState::PartiallyChecked);
  QCOMPARE(transition_control->checkState(), Qt::CheckState::PartiallyChecked);
  QCOMPARE(track->transition(), Transition::CrossFade);

  // QTest::mouseClick(transition_control, Qt::MouseButton::LeftButton);
  transition_control->setCheckState(Qt::CheckState::Checked);
  QCOMPARE(transition_control->checkState(), Qt::CheckState::Checked);
  QCOMPARE(track->transition(), Transition::FadeOutGapIn);

  auto status_control = track_controls->m_status_control;
  QCOMPARE(status_control->isEnabled(), true);
  QVERIFY(track->isPlaying());
  QCOMPARE(track->playerA()->playbackState(), QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(status_control->isChecked());
  QCOMPARE(status_control->text(), track->title());

  QTest::mouseClick(status_control, Qt::MouseButton::LeftButton);
  QVERIFY(!track->isPlaying());
  QCOMPARE(track->playerA()->playbackState(), QMediaPlayer::PlaybackState::PausedState);
  QVERIFY(!status_control->isChecked());

  QTest::mouseClick(status_control, Qt::MouseButton::LeftButton);
  QVERIFY(track->isPlaying());
  QCOMPARE(track->playerA()->playbackState(), QMediaPlayer::PlaybackState::PlayingState);
  QVERIFY(status_control->isChecked());
}

void TestTrackControls::testAudioFileBroken()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0) && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
  QSKIP("Test does not work");
#endif

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_broken;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto volume_control = track_controls->m_volume_control;
  QCOMPARE(volume_control->value(), 50);

  auto status_control = track_controls->m_status_control;
  QCOMPARE(status_control->isEnabled(), false);
  QVERIFY(!track->isPlaying());
  QCOMPARE(status_control->checkState(), Qt::CheckState::Unchecked);
  QCOMPARE(status_control->text(), track->title());
}

void TestTrackControls::testMenu()
{
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(QJsonObject(), QDir(), main_window);
  auto menu = track_controls->m_mouse_menu;
  auto actions = menu->actions();
  QCOMPARE(actions.at(0)->text(), "Edit Settings");
  QCOMPARE(actions.at(1)->text(), "Move Up");
  QCOMPARE(actions.at(2)->text(), "Move Down");
  QCOMPARE(actions.at(3)->text(), "Remove");
}

void TestTrackControls::testPauseAndResume()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_audio_ok;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();
  auto status_control = track_controls->m_status_control;

  auto test_playing_state = [&](auto* player_A, auto* player_B) {
    QCOMPARE(status_control->isChecked(), true);
    QCOMPARE(track->isPlaying(), true);
    QVERIFY(player_A->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

    track_controls->pausePlaying();
    QCOMPARE(status_control->isChecked(), true);
    QCOMPARE(track->isPlaying(), false);
    QVERIFY(player_A->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

    track_controls->resumePaused();
    QCOMPARE(status_control->isChecked(), true);
    QCOMPARE(track->isPlaying(), true);
    QVERIFY(player_A->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

    track_controls->pausePlaying();
    QTest::mouseClick(status_control, Qt::MouseButton::LeftButton);
    track_controls->resumePaused();
    QCOMPARE(status_control->isChecked(), false);
    QCOMPARE(track->isPlaying(), false);
    QVERIFY(player_A->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);

    QTest::mouseClick(status_control, Qt::MouseButton::LeftButton);
    QCOMPARE(status_control->isChecked(), true);
    QCOMPARE(track->isPlaying(), true);
    QVERIFY(player_A->playbackState() == QMediaPlayer::PlaybackState::PlayingState);
    QVERIFY(player_B->playbackState() != QMediaPlayer::PlaybackState::PlayingState);
  };

  // player A is active, player B is not
  test_playing_state(track->playerA(), track->playerB());

  QSignalSpy playbackStateA(track->playerA(), &QMediaPlayer::playbackStateChanged);
  QSignalSpy playbackStateB(track->playerB(), &QMediaPlayer::playbackStateChanged);
  track->playerA()->mediaPlayerPositionChanged(track->duration());
  track->playerA()->mediaPlayerStatusChanged(QMediaPlayer::MediaStatus::EndOfMedia);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
  QVERIFY(playbackStateA.wait());
  QVERIFY(playbackStateB.wait());
#endif
  // player B is active, player A is not
  test_playing_state(track->playerB(), track->playerA());
}

QTEST_MAIN(TestTrackControls)
#include "TestTrackControls.moc"
