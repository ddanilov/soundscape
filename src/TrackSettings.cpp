// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "TrackSettings.h"

#include "IconLabel.h"
#include "Player.h"
#include "PositionLabel.h"
#include "PositionSlider.h"
#include "Track.h"
#include "TrackControls.h"
#include "Version.h"

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
    m_position_slider_A(new PositionSlider),
    m_position_label_A(new PositionLabel),
    m_position_slider_B(new PositionSlider),
    m_position_label_B(new PositionLabel),
    m_gap_spin_box(new QDoubleSpinBox),
    m_random_gap_check_box(new QCheckBox),
    m_gap_max_spin_box(new QDoubleSpinBox)
{
  connect(m_track_controls, &TrackControls::updated, this, &TrackSettings::trackLoaded);

  connect(m_track->playerA(), &Player::positionChanged,
          this, [this](qint64 pos) { if(isVisible()) { playerPositionChanged(pos, Slider::PlayerA); } });
  connect(m_track->playerB(), &Player::positionChanged,
          this, [this](qint64 pos) { if(isVisible()) { playerPositionChanged(pos, Slider::PlayerB); } });

  setModal(false);

  m_box_layout->setAlignment(Qt::AlignmentFlag::AlignTop);

  addTrackTitle();
  addSlider(Slider::FadeIn);
  addSlider(Slider::FadeOut);
  addGap();

  auto spacing = fontInfo().pixelSize();
  m_box_layout->addSpacing(spacing);
  addSlider(Slider::PlayerA);
  addSlider(Slider::PlayerB);
}

void TrackSettings::trackLoaded()
{
  setWindowTitle(QString("%1: %2").arg(APP_TITLE, m_track->title()));
  setTrackProperties();
  setFade(Slider::FadeIn);
  setFade(Slider::FadeOut);
}

void TrackSettings::gapSpinBoxChanged(double value)
{
  if (value > m_gap_max_spin_box->value())
  {
    value = m_gap_max_spin_box->value();
    m_gap_spin_box->setValue(value);
  }
  m_track->setGap(value);
}

void TrackSettings::randomGapCheckBoxChanged(int state)
{
  m_track->setRandomGap(state == Qt::Checked);
}

void TrackSettings::gapMaxSpinBoxChanged(double value)
{
  if (value < m_gap_spin_box->value())
  {
    value = m_gap_spin_box->value();
    m_gap_max_spin_box->setValue(value);
  }
  m_track->setMaxGap(value);
}

void TrackSettings::addTrackTitle()
{
  m_track_label->setTextFormat(Qt::TextFormat::PlainText);
  m_track_duration->setToolTip(tr("track duration"));

  auto* widget = new QWidget;
  m_box_layout->addWidget(widget);
  auto* layout = new QHBoxLayout(widget);
  layout->addWidget(m_track_label);
  layout->addStretch(1);
  layout->addWidget(m_track_duration);
}

void TrackSettings::addSlider(Slider type)
{
  PositionSlider* slider;
  PositionLabel* label;
  QString tip;
  QString icon;

  if (type == Slider::FadeIn)
  {
    slider = m_fade_in_slider;
    label = m_fade_in_label;
    tip = tr("fade-in");
    icon = ":/icons/fade-in-label.svg";
  }
  else if (type == Slider::FadeOut)
  {
    slider = m_fade_out_slider;
    label = m_fade_out_label;
    tip = tr("fade-out");
    icon = ":/icons/fade-out-label.svg";
    slider->setInvertedAppearance(true);
  }
  else if (type == Slider::PlayerA)
  {
    slider = m_position_slider_A;
    label = m_position_label_A;
    tip = tr("player A");
    icon = ":/icons/position-label.svg";
  }
  else if (type == Slider::PlayerB)
  {
    slider = m_position_slider_B;
    label = m_position_label_B;
    tip = tr("player B");
    icon = ":/icons/position-label.svg";
  }

  if (type == Slider::FadeIn || type == Slider::FadeOut)
  {
    connect(slider, &QSlider::valueChanged, this, [this, type](int value) { fadeSliderChanged(value, type); });
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

void TrackSettings::addGap()
{
  auto* widget = new QWidget;
  m_box_layout->addWidget(widget);

  m_gap_spin_box->setDecimals(1);
  m_gap_spin_box->setMaximum(9999);

  m_gap_max_spin_box->setDecimals(1);
  m_gap_max_spin_box->setMaximum(9999);

  auto* layout = new QHBoxLayout(widget);
  layout->addWidget(new IconLabel(":/icons/gap-label.svg"));
  layout->addWidget(new QLabel(tr("gap")));
  layout->addWidget(m_gap_spin_box);
  layout->addWidget(new QLabel(tr("s")));

  layout->addWidget(new QLabel("   "));
  layout->addWidget(m_random_gap_check_box);
  layout->addWidget(new QLabel(tr("random up to")));
  layout->addWidget(m_gap_max_spin_box);
  layout->addWidget(new QLabel(tr("s")));

  layout->addStretch(1);
}

void TrackSettings::playerPositionChanged(qint64 pos, Slider type)
{
  PositionSlider* slider;
  PositionLabel* label;

  if (type == Slider::PlayerA)
  {
    slider = m_position_slider_A;
    label = m_position_label_A;
  }
  else if (type == Slider::PlayerB)
  {
    slider = m_position_slider_B;
    label = m_position_label_B;
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
  m_position_label_A->setMax(d);
  m_position_label_B->setMax(d);

  m_track_duration->setValue(d);
  m_position_label_A->setValue(0);
  m_position_label_B->setValue(0);

  m_gap_spin_box->setValue(m_track->gap());
  m_gap_max_spin_box->setValue(m_track->maxGap());
  m_random_gap_check_box->setChecked(m_track->randomGap());

  connect(m_gap_spin_box, &QDoubleSpinBox::valueChanged, this, &TrackSettings::gapSpinBoxChanged);
  connect(m_random_gap_check_box, &QCheckBox::stateChanged, this, &TrackSettings::randomGapCheckBoxChanged);
  connect(m_gap_max_spin_box, &QDoubleSpinBox::valueChanged, this, &TrackSettings::gapMaxSpinBoxChanged);

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

int TrackSettings::trackToSliderPosition(const qint64 pos, const PositionSlider* slider) const
{
  const auto p = static_cast<double>(pos);
  const auto d = static_cast<double>(m_track->duration());
  const auto m = static_cast<double>(slider->maximum());
  const auto x = static_cast<int>(p / d * m);
  return x;
}

void TrackSettings::fadeSliderChanged(int value, Slider type)
{
  PositionSlider* slider;
  PositionLabel* label;
  qint64 opposite_fade_duration;

  if (type == Slider::FadeIn)
  {
    slider = m_fade_in_slider;
    label = m_fade_in_label;
    opposite_fade_duration = m_track->fadeOutDuration();
  }
  else if (type == Slider::FadeOut)
  {
    slider = m_fade_out_slider;
    label = m_fade_out_label;
    opposite_fade_duration = m_track->fadeInDuration();
  }
  else
  {
    return;
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

  if (type == Slider::FadeIn)
  {
    m_track->setFadeInDuration(p);
  }
  else if (type == Slider::FadeOut)
  {
    m_track->setFadeOutDuration(p);
  }
}
