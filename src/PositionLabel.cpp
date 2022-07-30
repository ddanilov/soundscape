#include "PositionLabel.h"

PositionLabel::PositionLabel(QWidget* parent) :
    QLabel(parent)
{
  setTextFormat(Qt::PlainText);
}

void PositionLabel::setMax(double value)
{
  QString s;
  s.setNum(value, 'f', m_precision);
  m_field_width = static_cast<int>(s.size());
}

void PositionLabel::setValue(double value)
{
  setText(m_text_template.arg(value, m_field_width, 'f', m_precision, '0'));
}
