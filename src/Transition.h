// SPDX-FileCopyrightText: 2022-2023 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

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
