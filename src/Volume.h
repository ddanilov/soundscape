#pragma once

#include <QDial>

class Volume : public QDial
{
public:
  explicit Volume(QWidget* parent = nullptr);

  QSize sizeHint() const override;
  void setVolToolTip(double volume);

  QString m_tool_tip;
};
