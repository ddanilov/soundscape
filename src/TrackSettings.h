#pragma once

#include <QDialog>
#include <QLabel>
#include <QPointer>
#include <QVBoxLayout>

class Track;
class PositionLabel;
class PositionSlider;
class TrackControls;

enum class Slider
{
  FadeIn,
  FadeOut,
  Player,
};

class TrackSettings : public QDialog
{
  Q_OBJECT

public:
  explicit TrackSettings(TrackControls* parent = nullptr);

signals:
  void loaded();

private slots:
  void trackLoaded();

private:
  void addTrackTitle();
  template <Slider type> void addSlider();
  template <Slider type> void playerPositionChanged(qint64 pos);

  void setTrackProperties();
  void setFade(Slider type);

  int trackToSliderPosition(qint64 pos, PositionSlider* slider);

  template <Slider type> void fadeSliderChanged(int value);

  const TrackControls* m_track_controls;
  Track* m_track;

  QPointer<QVBoxLayout> m_box_layout;

  QPointer<QLabel> m_track_label;
  QPointer<PositionLabel> m_track_duration;

  QPointer<PositionSlider> m_fade_in_slider;
  QPointer<PositionLabel> m_fade_in_label;

  QPointer<PositionSlider> m_fade_out_slider;
  QPointer<PositionLabel> m_fade_out_label;

  QPointer<PositionSlider> m_position_slider;
  QPointer<PositionLabel> m_position_label;

  friend class TestTrackSettings;
};