#pragma once

#include <QCheckBox>

enum class Transition
{
  FadeOutIn = 0,
  CrossFade = 1,
  FadeOutGapIn = 2,
};

Transition convertTransition(Qt::CheckState state);
Qt::CheckState convertTransition(Transition transition);
