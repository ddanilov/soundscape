// SPDX-FileCopyrightText: 2022 Denis Danilov
// SPDX-License-Identifier: GPL-3.0-only

#include "MainWindow.h"
#include "Version.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QApplication::setApplicationName(APP_TITLE);
  QApplication::setApplicationVersion(APP_VERSION);

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();

  const QCommandLineOption load_option("load", "Load track list from file.", "path to file");
  parser.addOption(load_option);

  const QCommandLineOption minimize_option("minimize", "Minimize window to tray.");
  parser.addOption(minimize_option);

#if defined(Q_OS_MACOS)
  const QCommandLineOption tray_option("enable-tray", "Enable tray icon.");
#else
  const QCommandLineOption tray_option("disable-tray", "Disable tray icon.");
#endif
  parser.addOption(tray_option);

  parser.process(a);

  const QString file_name = parser.value(load_option);
  const bool minimize = parser.isSet(minimize_option);
#if defined(Q_OS_MACOS)
  const bool disable_tray = !parser.isSet(tray_option);
#else
  const bool disable_tray = parser.isSet(tray_option);
#endif

  MainWindow w(disable_tray);
  w.start(file_name, minimize);

  return QApplication::exec();
}
