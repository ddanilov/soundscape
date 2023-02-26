#include "Status.h"

#include <QFile>

Status::Status(QWidget* parent) :
    QCheckBox(parent)
{
  QFile file(":/styles/status.css");
  file.open(QIODevice::ReadOnly);
  QString style_template(file.readAll());
  m_playing_style = style_template.arg(":/icons/switch-on.svg", ":/icons/switch-off.svg");
  m_paused_style = style_template.arg(":/icons/switch-paused.svg", ":/icons/switch-off.svg");

  setPlayingStyle();

  updateToolTip(checkState());
  connect(this, &Status::stateChanged, this, &Status::updateToolTip);
}

void Status::setPlayingStyle()
{
  setStyleSheet(m_playing_style);
}

void Status::setPausedStyle()
{
  setStyleSheet(m_paused_style);
}

void Status::updateToolTip(int /*value*/)
{
  switch (checkState())
  {
    case Qt::CheckState::Unchecked:
      setToolTip("paused");
      break;
    case Qt::CheckState::Checked:
      setToolTip("playing");
      break;
  }
}
