#include "JsonRW.h"
#include "MainWindow.h"
#include "Player.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"
#include "Volume.h"

#include <QSignalSpy>
#include <QTest>

class TestTrackControls : public QObject
{
  Q_OBJECT

public:
  TestTrackControls(QObject* parent = nullptr) :
      QObject(parent),
      tmp_dir(),
      base_dir(tmp_dir.path()),
      file_name_media_ok("./"),
      file_name_media_broken("./")
  {}

private slots:
  void initTestCase();

  void testMediaFileOk();
  void testMediaFileBroken();
  void testMenu();
  void testPauseAndResume();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;

  QString file_name_media_ok;
  QString file_name_media_broken;
};

void TestTrackControls::initTestCase()
{
  QFile media_file_ok(":/media/sound_0100.wav");
  file_name_media_ok.append(QFileInfo(media_file_ok).fileName());
  file_name_media_ok = QDir::cleanPath(base_dir.absoluteFilePath(file_name_media_ok));
  QVERIFY(media_file_ok.copy(file_name_media_ok));

  QFile media_file_broken(":/media/sound_XXXX.wav");
  file_name_media_broken.append(QFileInfo(media_file_broken).fileName());
  file_name_media_broken = QDir::cleanPath(base_dir.absoluteFilePath(file_name_media_broken));
  QVERIFY(media_file_broken.copy(file_name_media_broken));
}

void TestTrackControls::testMediaFileOk()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_media_ok;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto volume_control = track_controls->m_volume_control;
  QCOMPARE(volume_control->value(), 50);

  auto status_control = track_controls->m_status_control;
  QCOMPARE(status_control->isEnabled(), true);
  QVERIFY(track->isPlaying());
  QVERIFY(status_control->isChecked());
  QCOMPARE(status_control->text(), track->title());

  QTest::mouseClick(status_control, Qt::LeftButton);
  QVERIFY(!track->isPlaying());
  QVERIFY(!status_control->isChecked());

  QTest::mouseClick(status_control, Qt::LeftButton);
  QVERIFY(track->isPlaying());
  QVERIFY(status_control->isChecked());
}

void TestTrackControls::testMediaFileBroken()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name_media_broken;
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
  QCOMPARE(status_control->checkState(), Qt::Unchecked);
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
  json[JsonRW::FileNameTag] = file_name_media_ok;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();
  auto status_control = track_controls->m_status_control;

  QCOMPARE(status_control->isChecked(), true);
  QCOMPARE(track->isPlaying(), true);
  QCOMPARE(track->player()->playbackState(), QMediaPlayer::PlayingState);

  track_controls->pausePlaying();
  QCOMPARE(status_control->isChecked(), true);
  QCOMPARE(track->isPlaying(), false);
  QCOMPARE(track->player()->playbackState(), QMediaPlayer::PausedState);

  track_controls->resumePaused();
  QCOMPARE(status_control->isChecked(), true);
  QCOMPARE(track->isPlaying(), true);
  QCOMPARE(track->player()->playbackState(), QMediaPlayer::PlayingState);

  track_controls->pausePlaying();
  QTest::mouseClick(status_control, Qt::LeftButton);
  track_controls->resumePaused();
  QCOMPARE(status_control->isChecked(), false);
  QCOMPARE(track->isPlaying(), false);
  QCOMPARE(track->player()->playbackState(), QMediaPlayer::PausedState);
}

QTEST_MAIN(TestTrackControls)
#include "TestTrackControls.moc"