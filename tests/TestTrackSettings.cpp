#include "JsonRW.h"
#include "MainWindow.h"
#include "PositionLabel.h"
#include "PositionSlider.h"
#include "Track.h"
#include "TrackControls.h"
#include "TrackSettings.h"

#include <QPointer>
#include <QSignalSpy>
#include <QTest>

class TestTrackSettings : public QObject
{
  Q_OBJECT

public:
  TestTrackSettings(QObject* parent = nullptr) :
      QObject(parent),
      tmp_dir(),
      base_dir(tmp_dir.path()),
      file_name("./")
  {}

private slots:
  void initTestCase();
  void init();

  void testInitialTrackProperties();
  void testPlayerPositionChanged();
  void testTrackToSliderPosition();
  void testFadeSliderChanged();

private:
  const QTemporaryDir tmp_dir;
  const QDir base_dir;
  QString file_name;
  QPointer<MainWindow> main_window;
  QPointer<TrackControls> track_controls;
  QPointer<TrackSettings> track_settings;
  Track* track;
};

void TestTrackSettings::initTestCase()
{
  QFile sound_file(":/media/sound_0100.wav");
  file_name.append(QFileInfo(sound_file).fileName());
  file_name = QDir::cleanPath(base_dir.absoluteFilePath(file_name));
  QVERIFY(sound_file.copy(file_name));
}

void TestTrackSettings::init()
{
  QJsonObject json;
  json[JsonRW::FileNameTag] = file_name;
  main_window = new MainWindow();
  track_controls = new TrackControls(json, QDir(), main_window);
  track_settings = new TrackSettings(track_controls);
  QSignalSpy loaded(track_settings, &TrackSettings::loaded);
  QVERIFY(loaded.wait());

  track = track_controls->track();
  track->pause();
}

void TestTrackSettings::testInitialTrackProperties()
{
  QCOMPARE(track_settings->m_track_label->text(), "sound_0100.wav");
  QCOMPARE(track_settings->m_track_duration->text(), "1.0 s");

  QCOMPARE(track_settings->m_fade_in_slider->value(), 125);
  QCOMPARE(track_settings->m_fade_in_label->text(), "0.3 s");

  QCOMPARE(track_settings->m_fade_out_slider->value(), 125);
  QCOMPARE(track_settings->m_fade_out_label->text(), "0.3 s");

  QCOMPARE(track_settings->m_position_slider->value(), 0);
  QCOMPARE(track_settings->m_position_label->text(), "0.0 s");
}

void TestTrackSettings::testPlayerPositionChanged()
{
  auto slider_value = track_settings->m_position_slider->value();
  QCOMPARE(slider_value, 0);
  auto slider_label = track_settings->m_position_label->text();
  QCOMPARE(slider_label, "0.0 s");

  auto position = track->duration();
  track_settings->playerPositionChanged<Slider::Player>(position);
  slider_value = track_settings->m_position_slider->value();
  QCOMPARE(slider_value, track_settings->m_fade_in_slider->maximum());
  slider_label = track_settings->m_position_label->text();
  QCOMPARE(slider_label, "1.0 s");

  position = track->duration() / 3;
  track_settings->playerPositionChanged<Slider::Player>(position);
  slider_value = track_settings->m_position_slider->value();
  QCOMPARE(slider_value, track_settings->m_fade_in_slider->maximum() / 3);
  slider_label = track_settings->m_position_label->text();
  QCOMPARE(slider_label, "0.3 s");

  position = 0;
  track_settings->playerPositionChanged<Slider::Player>(position);
  slider_value = track_settings->m_position_slider->value();
  QCOMPARE(slider_value, 0);
  slider_label = track_settings->m_position_label->text();
  QCOMPARE(slider_label, "0.0 s");
}

void TestTrackSettings::testTrackToSliderPosition()
{
  auto position = track->duration();
  auto slider_value = track_settings->trackToSliderPosition(position, track_settings->m_fade_in_slider);
  QCOMPARE(slider_value, track_settings->m_fade_in_slider->maximum());

  position = track->duration() / 2;
  slider_value = track_settings->trackToSliderPosition(position, track_settings->m_fade_in_slider);
  QCOMPARE(slider_value, track_settings->m_fade_in_slider->maximum() / 2);

  position = 0;
  slider_value = track_settings->trackToSliderPosition(position, track_settings->m_fade_in_slider);
  QCOMPARE(slider_value, 0);
}

void TestTrackSettings::testFadeSliderChanged()
{
  QCOMPARE(track_settings->m_fade_in_slider->value(), 125);
  QCOMPARE(track_settings->m_fade_in_label->text(), "0.3 s");
  QCOMPARE(track->fadeInDuration(), 250);

  track_settings->m_fade_in_slider->setValue(0);
  QCOMPARE(track_settings->m_fade_in_slider->value(), 0);
  QCOMPARE(track_settings->m_fade_in_label->text(), "0.0 s");
  QCOMPARE(track->fadeInDuration(), 0);

  track_settings->m_fade_in_slider->setValue(250);
  QCOMPARE(track_settings->m_fade_in_slider->value(), 250);
  QCOMPARE(track_settings->m_fade_in_label->text(), "0.5 s");
  QCOMPARE(track->fadeInDuration(), 500);

  // overlap of fade-in/out is prevented
  track_settings->m_fade_in_slider->setValue(450);
  QCOMPARE(track_settings->m_fade_in_slider->value(), 375);
  QCOMPARE(track_settings->m_fade_in_label->text(), "0.8 s");
  QCOMPARE(track->fadeInDuration(), 750);
}

QTEST_MAIN(TestTrackSettings)
#include "TestTrackSettings.moc"
