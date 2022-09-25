#pragma once

#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
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
  PlayerA,
  PlayerB,
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
  void gapSpinBoxChanged(double value);
  void randomGapCheckBoxChanged(int state);
  void gapMaxSpinBoxChanged(double value);

private:
  void addTrackTitle();
  void addSlider(Slider type);
  void addGap();

  void playerPositionChanged(qint64 pos, Slider type);

  void setTrackProperties();
  void setFade(Slider type);

  int trackToSliderPosition(qint64 pos, PositionSlider* slider);

  void fadeSliderChanged(int value, Slider type);

  const TrackControls* m_track_controls;
  Track* m_track;

  QPointer<QVBoxLayout> m_box_layout;

  QPointer<QLabel> m_track_label;
  QPointer<PositionLabel> m_track_duration;

  QPointer<PositionSlider> m_fade_in_slider;
  QPointer<PositionLabel> m_fade_in_label;

  QPointer<PositionSlider> m_fade_out_slider;
  QPointer<PositionLabel> m_fade_out_label;

  QPointer<PositionSlider> m_position_slider_A;
  QPointer<PositionLabel> m_position_label_A;

  QPointer<PositionSlider> m_position_slider_B;
  QPointer<PositionLabel> m_position_label_B;

  QPointer<QDoubleSpinBox> m_gap_spin_box;
  QPointer<QCheckBox> m_random_gap_check_box;
  QPointer<QDoubleSpinBox> m_gap_max_spin_box;

  friend class TestTrackSettings;
};
