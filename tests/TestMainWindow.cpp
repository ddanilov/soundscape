#include "MainWindow.h"
#include "Status.h"
#include "Track.h"
#include "TrackControls.h"
#include "Volume.h"

#include <QTemporaryFile>
#include <QTest>

class TestMainWindow : public QObject
{
  Q_OBJECT

private slots:
  void testTrackFromMedia();
  void testSaveTracksToJson();
  void testLoadTracksFromJson();

  void testMoveTrackUp();
  void testMoveTrackDown();
  void testRemoveTrack();
};

void TestMainWindow::testTrackFromMedia()
{
  MainWindow window;
  QVERIFY(window.m_box_layout->count() == 1);
  // menu info is the first item in the box layout
  auto* menu_info = dynamic_cast<QLabel*>(window.m_box_layout->itemAt(0)->widget());
  QVERIFY(menu_info != nullptr);

  window.addTrackFromMedia(QString("/tmp/sound_01.mp3"));
  QVERIFY(window.m_box_layout->count() == 2);
  // menu info is still the first item in the box layout
  menu_info = dynamic_cast<QLabel*>(window.m_box_layout->itemAt(0)->widget());
  QVERIFY(menu_info != nullptr);
  // added track is the second item
  auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(1)->widget());
  QVERIFY(track_control != nullptr);
  auto* track = track_control->findChild<Track*>();
  QCOMPARE(track->title(), "sound_01");
  QCOMPARE(track->volume(), 0.50);
  QVERIFY(track->isPlaying());

  window.addTrackFromMedia(QString("sound_02.mp3"));
  QVERIFY(window.m_box_layout->count() == 3);
  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(2)->widget());
  track = track_control->findChild<Track*>();
  QVERIFY(track_control != nullptr);
  QCOMPARE(track->title(), "sound_02");
  QCOMPARE(track->volume(), 0.50);
  QVERIFY(track->isPlaying());
}

void TestMainWindow::testSaveTracksToJson()
{
  QTemporaryFile file;
  file.open();
  file.write(QString("=== abc ===").toUtf8());
  file.close();
  const QFileInfo file_info(file.fileName());
  const auto& base_dir = file_info.dir().absolutePath();

  MainWindow window;
  window.addTrackFromMedia(QString(base_dir + "/sound_01.mp3"));
  window.addTrackFromMedia(QString(base_dir + "/../data1/sound_02.mp3"));
  window.addTrackFromMedia(QString(base_dir + "/../data2/sound_03.mp3"));

  auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(1)->widget());
  auto* track = track_control->findChild<Track*>();
  track->setVolume(0.11);

  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(2)->widget());
  track = track_control->findChild<Track*>();
  track->setVolume(0.21);

  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(3)->widget());
  track = track_control->findChild<Track*>();
  track->setVolume(0.32);

  file.open();
  window.saveTracksToJson(file);
  file.close();

  QString json;
  QString jsonExpected;

  file.open();
  json = file.readAll();
  file.close();
  jsonExpected = R"({
    "tracks": [
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "sound_01.mp3",
            "playing": true,
            "volume": 0.11
        },
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "../data1/sound_02.mp3",
            "playing": true,
            "volume": 0.21
        },
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "../data2/sound_03.mp3",
            "playing": true,
            "volume": 0.32
        }
    ]
}
)";
  QCOMPARE(json, jsonExpected);

  // move the last track up
  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(3)->widget());
  track_control->moveUp();

  file.open();
  window.saveTracksToJson(file);
  file.close();

  file.open();
  json = file.readAll();
  file.close();
  jsonExpected = R"({
    "tracks": [
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "sound_01.mp3",
            "playing": true,
            "volume": 0.11
        },
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "../data2/sound_03.mp3",
            "playing": true,
            "volume": 0.32
        },
        {
            "fadeInDuration": -1,
            "fadeOutDuration": -1,
            "fileName": "../data1/sound_02.mp3",
            "playing": true,
            "volume": 0.21
        }
    ]
}
)";
  QCOMPARE(json, jsonExpected);
}

void TestMainWindow::testLoadTracksFromJson()
{
  QTemporaryFile file;
  file.open();
  QString json(R"({
    "tracks": [
        {
            "fileName": "sound_01.mp3",
            "playing": true,
            "volume": 0.51
        },
        {
            "fileName": "../data1/sound_02.mp3",
            "playing": false,
            "volume": 0.52
        },
        {
            "fileName": "../data2/sound_03.mp3",
            "volume": 0.53
        }
    ]
}
)");
  file.write(json.toUtf8());
  file.close();

  MainWindow window;

  file.open();
  window.loadTracksFromJson(file);
  file.close();

  QVERIFY(window.m_box_layout->count() == 4);
  // menu info is the first item in the box layout
  auto* menu_info = dynamic_cast<QLabel*>(window.m_box_layout->itemAt(0)->widget());
  QVERIFY(menu_info != nullptr);
  // followed by the three tracks
  auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(1)->widget());
  QVERIFY(track_control != nullptr);
  auto* track = track_control->findChild<Track*>();
  QCOMPARE(track->title(), "sound_01");
  QCOMPARE(track->volume(), 0.51);
  QVERIFY(track->isPlaying());
  //
  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(2)->widget());
  QVERIFY(track_control != nullptr);
  track = track_control->findChild<Track*>();
  QCOMPARE(track->title(), "sound_02");
  QCOMPARE(track->volume(), 0.52);
  QVERIFY(!track->isPlaying());
  //
  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(3)->widget());
  QVERIFY(track_control != nullptr);
  track = track_control->findChild<Track*>();
  QCOMPARE(track->title(), "sound_03");
  QCOMPARE(track->volume(), 0.53);
  QVERIFY(track->isPlaying());
}

void TestMainWindow::testMoveTrackUp()
{
  MainWindow window;

  window.addTrackFromMedia(QString("track_01.mp3"));
  window.addTrackFromMedia(QString("track_02.mp3"));
  window.addTrackFromMedia(QString("track_03.mp3"));

  auto move_track = [&](int index) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    track_control->moveUp();
  };

  auto check_track = [&](int index, const QString& title) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    auto* track = track_control->findChild<Track*>();
    QCOMPARE(track->title(), title);
  };

  check_track(1, "track_01");
  check_track(2, "track_02");
  check_track(3, "track_03");

  move_track(1);
  check_track(1, "track_01");
  check_track(2, "track_02");
  check_track(3, "track_03");

  move_track(2);
  check_track(1, "track_02");
  check_track(2, "track_01");
  check_track(3, "track_03");

  move_track(3);
  check_track(1, "track_02");
  check_track(2, "track_03");
  check_track(3, "track_01");
}

void TestMainWindow::testMoveTrackDown()
{
  MainWindow window;

  window.addTrackFromMedia(QString("track_01.mp3"));
  window.addTrackFromMedia(QString("track_02.mp3"));
  window.addTrackFromMedia(QString("track_03.mp3"));

  auto move_track = [&](int index) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    track_control->moveDown();
  };

  auto check_track = [&](int index, const QString& title) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    auto* track = track_control->findChild<Track*>();
    QCOMPARE(track->title(), title);
  };

  check_track(1, "track_01");
  check_track(2, "track_02");
  check_track(3, "track_03");

  move_track(1);
  check_track(1, "track_02");
  check_track(2, "track_01");
  check_track(3, "track_03");

  move_track(2);
  check_track(1, "track_02");
  check_track(2, "track_03");
  check_track(3, "track_01");

  move_track(3);
  check_track(1, "track_02");
  check_track(2, "track_03");
  check_track(3, "track_01");
}

void TestMainWindow::testRemoveTrack()
{
  MainWindow window;

  window.addTrackFromMedia(QString("track_01.mp3"));
  window.addTrackFromMedia(QString("track_02.mp3"));
  window.addTrackFromMedia(QString("track_03.mp3"));

  auto remove_track = [&](int index) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    track_control->remove();
  };

  auto check_track = [&](int index, const QString& title) {
    auto* track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(index)->widget());
    auto* track = track_control->findChild<Track*>();
    QCOMPARE(track->title(), title);
  };

  check_track(1, "track_01");
  check_track(2, "track_02");
  check_track(3, "track_03");
  QVERIFY(window.m_box_layout->count() == 3 + 1);

  remove_track(1);
  check_track(1, "track_02");
  check_track(2, "track_03");
  QVERIFY(window.m_box_layout->count() == 2 + 1);

  remove_track(2);
  check_track(1, "track_02");
  QVERIFY(window.m_box_layout->count() == 1 + 1);

  remove_track(1);
  QVERIFY(window.m_box_layout->count() == 1);
}

QTEST_MAIN(TestMainWindow)
#include "TestMainWindow.moc"
