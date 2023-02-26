#pragma once

#include <QCheckBox>

class TransitionIcon : public QCheckBox
{
public:
  explicit TransitionIcon(QWidget* parent = nullptr);

private:
  void updateToolTip(int value);
};
