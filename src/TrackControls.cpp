#include "TrackControls.h"

#include "MainWindow.h"
#include "Status.h"
#include "Track.h"
#include "TrackSettings.h"
#include "TransitionIcon.h"
#include "Volume.h"

#include <QFile>
#include <QMouseEvent>
#include <QUuid>

TrackControls::TrackControls(const QJsonObject& json, const QDir& base_dir, MainWindow* parent) :
    QFrame(parent),
    m_main_window(parent),
    m_layout(new QHBoxLayout(this)),
    m_mouse_menu(new QMenu(this)),
    m_track(new Track(this)),
    m_volume_control(new Volume(this)),
    m_transition_control(new TransitionIcon(this)),
    m_status_control(new Status(this)),
    m_settings(new TrackSettings(this))
{
  setObjectName(QUuid::createUuid().toString());

  auto spacing = fontInfo().pixelSize() / 2;
  QFile file(":/styles/track-controls.css");
  file.open(QIODevice::OpenModeFlag::ReadOnly);
  const QString& style = QString(file.readAll()).arg(spacing).arg(spacing / 2);
  setStyleSheet(style);

  addItemsToMenu(m_mouse_menu);
  m_mouse_menu->addSeparator();
  m_main_window->addItemsToMenu(m_mouse_menu);

  connect(m_track, &Track::loaded, this, &TrackControls::trackLoaded);
  connect(m_track, &Track::errorOccurred, this, &TrackControls::playerError);

  setupControls();
  disableControls();

  m_track->fromJsonObject(json, base_dir);
}

Track* TrackControls::track() const
{
  return m_track;
}

void TrackControls::pausePlaying()
{
  if (m_status_control->isChecked() && m_track->isPlaying())
  {
    m_track->pause();
    m_status_control->setPausedStyle();
  }
}

void TrackControls::resumePaused()
{
  if (m_status_control->isChecked() && !m_track->isPlaying())
  {
    m_track->play();
    m_status_control->setPlayingStyle();
  }
}

void TrackControls::moveUp()
{
  m_main_window->moveTrackUp(objectName());
}

void TrackControls::moveDown()
{
  m_main_window->moveTrackDown(objectName());
}

void TrackControls::remove()
{
  m_main_window->removeTrack(objectName());
}

void TrackControls::volumeChanged(int value)
{
  const auto val = static_cast<double>(value);
  const auto max = static_cast<double>(m_volume_control->maximum());
  const auto volume = val / max;
  m_track->setVolume(volume);
  m_volume_control->setVolToolTip(volume);
}

void TrackControls::transitionChanged(int state)
{
  m_track->setTransition(convertTransition(static_cast<Qt::CheckState>(state)));
}

void TrackControls::statusChanged(int state)
{
  switch (state)
  {
    case Qt::CheckState::Unchecked:
      m_track->pause();
      break;
    case Qt::CheckState::Checked:
      m_track->play();
      m_status_control->setPlayingStyle();
      break;
  }
}

void TrackControls::trackLoaded()
{
  if (m_track->errors().empty())
  {
    updateControls();
    enableControls();
    emit updated();
  }
}

void TrackControls::playerError()
{
  if (m_track->errors().size() == 1)
  {
    disableControls();
    updateControls();

    m_transition_control->setToolTip(m_transition_control->toolTip().append(": ").append(m_track->errors().front()));
    m_transition_control->installEventFilter(this);

    m_status_control->setToolTip(m_status_control->toolTip().append(": ").append(m_track->errors().front()));
    m_status_control->installEventFilter(this);

    emit updated();
  }
}

void TrackControls::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MouseButton::RightButton)
  {
    m_mouse_menu->exec(QCursor::pos());
  }
}

bool TrackControls::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == m_transition_control ||
      watched == m_status_control)
  {
    if (event->type() == QEvent::Type::MouseButtonPress)
    {
      auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
      mousePressEvent(mouseEvent);
      return true;
    }
    return false;
  }

  return QFrame::eventFilter(watched, event);
}

void TrackControls::addItemsToMenu(QMenu* menu) const
{
  auto* show_settings = menu->addAction(tr("Edit Settings"));
  connect(show_settings, &QAction::triggered, this, [this]() { m_settings->show(); });

  auto* move_track_up = menu->addAction(tr("Move Up"));
  connect(move_track_up, &QAction::triggered, this, &TrackControls::moveUp);

  auto* move_track_down = menu->addAction(tr("Move Down"));
  connect(move_track_down, &QAction::triggered, this, &TrackControls::moveDown);

  auto* remove_track = menu->addAction(tr("Remove"));
  connect(remove_track, &QAction::triggered, this, &TrackControls::remove);
}

void TrackControls::setupControls()
{
  auto spacing = fontInfo().pixelSize() / 2;

  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->addWidget(m_volume_control, 0, Qt::AlignmentFlag::AlignLeft);
  m_layout->addSpacing(spacing);
  m_layout->addWidget(m_transition_control, 0, Qt::AlignmentFlag::AlignLeft);
  m_layout->addSpacing(spacing);
  m_layout->addWidget(m_status_control, 1, Qt::AlignmentFlag::AlignLeft);

  setLayout(m_layout);

  connect(m_volume_control, &QDial::valueChanged, this, &TrackControls::volumeChanged);
  connect(m_transition_control, &QCheckBox::stateChanged, this, &TrackControls::transitionChanged);
  connect(m_status_control, &QCheckBox::stateChanged, this, &TrackControls::statusChanged);
}

void TrackControls::disableControls()
{
  m_volume_control->setDisabled(true);
  m_transition_control->setDisabled(true);
  m_status_control->setDisabled(true);
}

void TrackControls::enableControls()
{
  m_volume_control->setEnabled(true);
  m_transition_control->setEnabled(true);
  m_status_control->setEnabled(true);
}

void TrackControls::updateControls()
{
  const auto volume = m_track->volume();
  const auto max = static_cast<double>(m_volume_control->maximum());
  const auto val = static_cast<int>(volume * max);
  m_volume_control->setValue(val);

  m_transition_control->setCheckState(convertTransition(m_track->transition()));
  m_transition_control->setToolTip(m_track->fileName());

  m_status_control->setChecked(m_track->isPlaying());
  m_status_control->setText(m_track->title());
  m_status_control->setToolTip(m_track->fileName());
}

Transition TrackControls::convertTransition(const Qt::CheckState state)
{
  switch (state)
  {
    case Qt::CheckState::Unchecked:
      return Transition::FadeOutIn;
    case Qt::CheckState::Checked:
      return Transition::CrossFade;
  }
  return Transition::FadeOutIn;
}

Qt::CheckState TrackControls::convertTransition(const Transition transition)
{
  switch (transition)
  {
    case Transition::FadeOutIn:
      return Qt::CheckState::Unchecked;
    case Transition::CrossFade:
      return Qt::CheckState::Checked;
  }
  return Qt::CheckState::Unchecked;
}
