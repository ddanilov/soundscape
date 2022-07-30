#include "JsonRW.h"
#include "MainWindow.h"
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
      base_dir(tmp_dir.path())
  {}

private slots:
  void testMediaFileOk();
  void testMediaFileBroken();
  void testMenu();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;
};

void TestTrackControls::testMediaFileOk()
{
  QFile sound_file(":/media/sound_0100.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto volume_control = track_controls->m_volume_control;
  QCOMPARE(volume_control->value(), 50);

  auto status_control = track_controls->m_status_control;
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
  QFile sound_file(":/media/sound_XXXX.wav");
  QString file_name("./");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));

  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  auto* main_window = new MainWindow();
  auto* track_controls = new TrackControls(json, QDir(), main_window);
  QSignalSpy updated(track_controls, &TrackControls::updated);
  QVERIFY(updated.wait());

  auto* track = track_controls->track();

  auto volume_control = track_controls->m_volume_control;
  QCOMPARE(volume_control->value(), 50);

  auto status_control = track_controls->m_status_control;
  QVERIFY(status_control->isTristate());

  QVERIFY(!track->isPlaying());
  QCOMPARE(status_control->checkState(), Qt::PartiallyChecked);
  QCOMPARE(status_control->text(), track->title());

  QTest::mouseClick(status_control, Qt::LeftButton);
  QVERIFY(!track->isPlaying());
  QCOMPARE(status_control->checkState(), Qt::PartiallyChecked);
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

QTEST_MAIN(TestTrackControls)
#include "TestTrackControls.moc"
