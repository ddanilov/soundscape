# soundscape-qt

[![GitHub release](https://img.shields.io/github/release/ddanilov/soundscape-qt)](https://github.com/ddanilov/soundscape-qt/releases/)

[**soundscape-qt**](https://github.com/ddanilov/soundscape-qt) is an open source
desktop application to play a mix of sounds, e.g. natural sounds produced by
animals or wind and water.

![](screenshots/overview.png)

## Usage

**soundscape-qt** comes with two example soundscapes. Use mouse right-click in
the main window or on the tray icon to access application menu where you can add
or remove soundtracks and change their settings.

Use the `Quit` item from the application menu to finish the application.
Pressing the close window button will just minimize it to the tray.

[Freesound](https://freesound.org/) is a good source of sounds for your own
soundscapes.

## Installation

Prebuilt binaries for Windows, macOS and Linux are available from the
[Releases page](https://github.com/ddanilov/soundscape-qt/releases).

On Linux you may need to install additional packages in order to run the
application. Most likely XCB util-cursor module is missing. On Debian-based
systems you can install it with

    sudo apt-get install libxcb-cursor0

and on RPM-based sytems with

    sudo dnf install xcb-util-cursor
