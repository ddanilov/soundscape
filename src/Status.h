#pragma once

#include <QCheckBox>

class Status : public QCheckBox
{
public:
  explicit Status(QWidget* parent = nullptr);
  void setPlayingStyle();
  void setPausedStyle();

private:
  QString m_playing_style;
  QString m_paused_style;
};
