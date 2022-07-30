#include "TrackSettings.h"

#include "IconLabel.h"
#include "Player.h"
#include "PositionLabel.h"
#include "PositionSlider.h"
#include "Track.h"
#include "TrackControls.h"

TrackSettings::TrackSettings(TrackControls* parent) :
    QDialog(parent),
    m_track_controls(parent),
    m_track(m_track_controls->track()),
    m_box_layout(new QVBoxLayout(this)),
    m_track_label(new QLabel),
    m_track_duration(new PositionLabel),
    m_fade_in_slider(new PositionSlider),
    m_fade_in_label(new PositionLabel),
    m_fade_out_slider(new PositionSlider),
    m_fade_out_label(new PositionLabel),
    m_position_slider(new PositionSlider),
    m_position_label(new PositionLabel)
{
  connect(m_track_controls, &TrackControls::updated, this, &TrackSettings::trackLoaded);

  connect(m_track->player(), &Player::positionChanged,
          this, [this](qint64 pos) { if(isVisible()) { playerPositionChanged<Slider::Player>(pos); } });

  setModal(false);

  m_box_layout->setAlignment(Qt::AlignTop);

  addTrackTitle();
  addSlider<Slider::FadeIn>();
  addSlider<Slider::FadeOut>();

  auto spacing = fontInfo().pixelSize();
  m_box_layout->addSpacing(spacing);
  addSlider<Slider::Player>();
}

void TrackSettings::trackLoaded()
{
  setTrackProperties();
  setFade(Slider::FadeIn);
  setFade(Slider::FadeOut);
}

void TrackSettings::addTrackTitle()
{
  m_track_label->setTextFormat(Qt::PlainText);
  m_track_duration->setToolTip(tr("track duration"));

  auto* widget = new QWidget;
  m_box_layout->addWidget(widget);
  auto* layout = new QHBoxLayout(widget);
  layout->addWidget(m_track_label);
  layout->addStretch(1);
  layout->addWidget(m_track_duration);
}

template <Slider type>
void TrackSettings::addSlider()
{
  PositionSlider* slider;
  PositionLabel* label;
  QString tip;
  QString icon;

  if constexpr (type == Slider::FadeIn)
  {
    slider = m_fade_in_slider;
    label = m_fade_in_label;
    tip = tr("fade-in");
    icon = ":/icons/fade-in-label.svg";
  }
  else if constexpr (type == Slider::FadeOut)
  {
    slider = m_fade_out_slider;
    label = m_fade_out_label;
    tip = tr("fade-out");
    icon = ":/icons/fade-out-label.svg";
    slider->setInvertedAppearance(true);
  }
  else if constexpr (type == Slider::Player)
  {
    slider = m_position_slider;
    label = m_position_label;
    tip = tr("player");
    icon = ":/icons/position-label.svg";
  }

  if constexpr (type == Slider::FadeIn || type == Slider::FadeOut)
  {
    connect(slider, &QSlider::valueChanged, this, [this](int value) { fadeSliderChanged<type>(value); });
  }
  else
  {
    slider->setEnabled(false);
  }

  auto* icon_label = new IconLabel(icon);

  icon_label->setToolTip(tip);
  slider->setToolTip(tip);
  label->setToolTip(tip);

  auto* widget = new QWidget;
  m_box_layout->addWidget(widget);

  auto* layout = new QHBoxLayout(widget);
  layout->addWidget(icon_label);
  layout->addWidget(slider);
  layout->addWidget(label);
}

template <Slider type>
void TrackSettings::playerPositionChanged(qint64 pos)
{
  PositionSlider* slider;
  PositionLabel* label;

  if constexpr (type == Slider::Player)
  {
    slider = m_position_slider;
    label = m_position_label;
  }
  else
  {
    return;
  }

  const auto v = trackToSliderPosition(pos, slider);
  slider->setValue(v);
  const auto p = static_cast<double>(pos);
  const auto x = p / 1000;
  label->setValue(x);
}

void TrackSettings::setTrackProperties()
{
  m_track_label->setText(m_track->fileName());

  const auto d = static_cast<double>(m_track->duration()) / 1000;
  m_track_duration->setMax(d);
  m_fade_in_label->setMax(d);
  m_fade_out_label->setMax(d);
  m_position_label->setMax(d);

  m_track_duration->setValue(d);
  m_position_label->setValue(0);

  emit loaded();
}

void TrackSettings::setFade(Slider type)
{
  const qint64 duration = type == Slider::FadeIn ? m_track->fadeInDuration() : m_track->fadeOutDuration();
  PositionSlider* slider = type == Slider::FadeIn ? m_fade_in_slider : m_fade_out_slider;
  PositionLabel* label = type == Slider::FadeIn ? m_fade_in_label : m_fade_out_label;

  const auto x = trackToSliderPosition(duration, slider);
  slider->setValue(x);

  const auto v = static_cast<double>(duration) / 1000;
  label->setValue(v);
}

int TrackSettings::trackToSliderPosition(qint64 pos, PositionSlider* slider)
{
  const auto p = static_cast<double>(pos);
  const auto d = static_cast<double>(m_track->duration());
  const auto m = static_cast<double>(slider->maximum());
  const auto x = static_cast<int>(p / d * m);
  return x;
}

template <Slider type>
void TrackSettings::fadeSliderChanged(int value)
{
  PositionSlider* slider;
  PositionLabel* label;
  qint64 opposite_fade_duration;

  if constexpr (type == Slider::FadeIn)
  {
    slider = m_fade_in_slider;
    label = m_fade_in_label;
    opposite_fade_duration = m_track->fadeOutDuration();
  }
  else
  {
    slider = m_fade_out_slider;
    label = m_fade_out_label;
    opposite_fade_duration = m_track->fadeInDuration();
  }

  const auto v = static_cast<double>(value);
  const auto m = static_cast<double>(slider->maximum());
  const auto d = static_cast<double>(m_track->duration());
  const auto f = v / m * d;
  const auto p = static_cast<qint64>(f);
  const auto p0 = m_track->duration() - opposite_fade_duration;
  if (p > p0)
  {
    const auto v1 = trackToSliderPosition(p0, slider);
    slider->setValue(v1);
    return;
  }
  label->setValue(f / 1000);

  if constexpr (type == Slider::FadeIn)
  {
    m_track->setFadeInDuration(p);
  }
  else
  {
    m_track->setFadeOutDuration(p);
  }
}
