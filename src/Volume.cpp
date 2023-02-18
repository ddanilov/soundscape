#include "Volume.h"

#include <QPainter>

Volume::Volume(QWidget* parent) :
    QDial(parent),
    m_tool_tip(),
    m_min_angle(40.0),
    m_max_angle(360.0 - m_min_angle)
{
  setNotchesVisible(true);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  setMinimum(0);
  setMaximum(100);

  const auto vol = value() / static_cast<double>(maximum());
  setVolToolTip(vol);
}

QSize Volume::sizeHint() const
{
  return {10, 10};
}

void Volume::setVolToolTip(double volume)
{
  m_tool_tip.setNum(volume, 'f', 2);
  setToolTip(m_tool_tip);
}

void Volume::paintEvent(QPaintEvent* /*event*/)
{
  QPainter painter(this);
  painter.save();
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(Qt::NoBrush);

  const auto& pal = palette();

  const auto width = static_cast<double>(rect().width());
  const auto height = static_cast<double>(rect().height());
  const auto size = 0.8 * std::min(width, height);
  const auto radius = 0.5 * size;
  const auto thickness = std::max(0.05 * radius, 1.0);
  const auto start_angle = static_cast<int>(-90.0 - m_min_angle);

  const auto& base_rect = QRectF((width - size) / 2, (height - size) / 2, size, size);
  const auto& base_center = base_rect.center();

  painter.setPen(QPen(QColor(128, 128, 128), thickness, Qt::SolidLine, Qt::RoundCap));
  painter.drawArc(base_rect, start_angle * 16, -static_cast<int>(m_max_angle - m_min_angle) * 16);

  painter.setPen(QPen(QColor(0, 128, 0), 3.0 * thickness, Qt::SolidLine, Qt::RoundCap));
  painter.drawArc(base_rect, start_angle * 16, -static_cast<int>(arcLength() * 16));

  const auto angle = (start_angle - arcLength()) * M_PI / 180;
  const auto& rt = (radius - thickness) * QPointF(std::cos(-angle), std::sin(-angle));
  const auto& r1 = base_center + 0.6 * rt;
  const auto& r2 = base_center + 0.8 * rt;
  painter.drawLine(r1, r2);

  painter.restore();
}

double Volume::fraction() const
{
  return static_cast<double>(sliderPosition() - minimum()) / (maximum() - minimum());
}

double Volume::arcLength() const
{
  return (m_max_angle - m_min_angle) * fraction();
}
