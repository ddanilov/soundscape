#include "MainWindow.h"
#include "Track.h"
#include "TrackControls.h"

#include <QTemporaryFile>
#include <QTest>

class TestMainWindow : public QObject
{
  Q_OBJECT

private slots:
  void testTrackFromMedia();
  void testSaveTracksToJson();
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

  window.addTrackFromMedia(QString("sound_02.mp3"));
  QVERIFY(window.m_box_layout->count() == 3);
  track_control = dynamic_cast<TrackControls*>(window.m_box_layout->itemAt(2)->widget());
  QVERIFY(track_control != nullptr);
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

  file.open();
  window.saveTracksToJson(file);
  file.close();

  file.open();
  QString json(file.readAll());
  QString jsonExpected(R"({
    "tracks": [
        {
            "fileName": "sound_01.mp3",
            "volume": 0.5
        },
        {
            "fileName": "../data1/sound_02.mp3",
            "volume": 0.5
        },
        {
            "fileName": "../data2/sound_03.mp3",
            "volume": 0.5
        }
    ]
}
)");
  QCOMPARE(json, jsonExpected);
  file.close();
}

QTEST_MAIN(TestMainWindow)
#include "TestMainWindow.moc"
