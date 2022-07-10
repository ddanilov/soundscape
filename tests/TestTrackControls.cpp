#include "JsonRW.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"

#include <QSignalSpy>
#include <QTest>

class TestTrackControls : public QObject
{
  Q_OBJECT

private slots:
  void testStatus();
};

void TestTrackControls::testStatus()
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
  auto* track = track_controls->track();
  QSignalSpy loaded(track, &Track::loaded);
  QVERIFY(loaded.wait());

  auto* status_control = track_controls->findChild<Status*>();

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

QTEST_MAIN(TestTrackControls)
#include "TestTrackControls.moc"
