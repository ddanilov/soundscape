#include "PositionSlider.h"

PositionSlider::PositionSlider(QWidget* parent) :
    QSlider(parent)
{
  setOrientation(Qt::Horizontal);
  setRange(0, 500);
}
