#include "MainWindow.h"
#include "Track.h"
#include "TrackControls.h"

#include <QTest>

class TestMainWindow : public QObject
{
  Q_OBJECT

private slots:
  void testTrackFromMedia();
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

QTEST_MAIN(TestMainWindow)
#include "TestMainWindow.moc"
