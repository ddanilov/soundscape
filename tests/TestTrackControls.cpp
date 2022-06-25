#include "JsonRW.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"

#include <QTest>

class TestTrackControls : public QObject
{
  Q_OBJECT

private slots:
  void testStatus();
};

void TestTrackControls::testStatus()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = "sound_01";
  auto* track_controls = new TrackControls(json, QDir());
  auto* track = track_controls->track();
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
