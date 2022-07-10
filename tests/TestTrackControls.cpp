#include "JsonRW.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"
#include "Volume.h"

#include <QSignalSpy>
#include <QTest>

class TestTrackControls : public QObject
{
  Q_OBJECT

private slots:
  void testMediaFileOk();
  void testMediaFileBroken();
};

void TestTrackControls::testMediaFileOk()
{
  const QTemporaryDir tmp_dir;
  const auto& base_dir = QDir(tmp_dir.path());

  QFile sound_file(":/media/sound_0100.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* track_controls = new TrackControls(json, QDir());
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto* volume_control = track_controls->findChild<Volume*>();
  QCOMPARE(volume_control->value(), 50);

  auto* status_control = track_controls->findChild<Status*>();
  QVERIFY(!status_control->isTristate());

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
  const QTemporaryDir tmp_dir;
  const auto& base_dir = QDir(tmp_dir.path());

  QFile sound_file(":/media/sound_XXXX.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* track_controls = new TrackControls(json, QDir());
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto* volume_control = track_controls->findChild<Volume*>();
  QCOMPARE(volume_control->value(), 50);

  auto* status_control = track_controls->findChild<Status*>();
  QVERIFY(status_control->isTristate());

  QVERIFY(!track->isPlaying());
  QCOMPARE(status_control->checkState(), Qt::PartiallyChecked);
  QCOMPARE(status_control->text(), track->title());

  QTest::mouseClick(status_control, Qt::LeftButton);
  QVERIFY(!track->isPlaying());
  QCOMPARE(status_control->checkState(), Qt::PartiallyChecked);
}

QTEST_MAIN(TestTrackControls)
#include "TestTrackControls.moc"
