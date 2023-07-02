// SPDX-FileCopyrightText: 2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "Transition.h"

Transition convertTransition(Qt::CheckState state)
{
  switch (state)
  {
    case Qt::CheckState::Unchecked:
      return Transition::FadeOutIn;
    case Qt::CheckState::PartiallyChecked:
      return Transition::CrossFade;
    case Qt::CheckState::Checked:
      return Transition::FadeOutGapIn;
  }
  return Transition::FadeOutIn;
}

Qt::CheckState convertTransition(Transition transition)
{
  switch (transition)
  {
    case Transition::FadeOutIn:
      return Qt::CheckState::Unchecked;
    case Transition::CrossFade:
      return Qt::CheckState::PartiallyChecked;
    case Transition::FadeOutGapIn:
      return Qt::CheckState::Checked;
  }
  return Qt::CheckState::Unchecked;
}
