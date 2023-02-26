#pragma once

#include <QDial>

class Volume : public QDial
{
public:
  explicit Volume(QWidget* parent = nullptr);

  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  double fraction() const;
  double arcLength() const;
  void updateToolTip(int value);

  const QString m_tool_tip_template;
  double m_min_angle;
  double m_max_angle;
};
