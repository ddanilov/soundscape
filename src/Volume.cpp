#include "Volume.h"

Volume::Volume(QWidget* parent) :
    QDial(parent),
    m_tool_tip()
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
