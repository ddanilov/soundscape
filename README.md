# Soundscape

[![GitHub release](https://img.shields.io/github/release/ddanilov/soundscape-qt)](https://github.com/ddanilov/soundscape-qt/releases/)

[**Soundscape**](https://github.com/ddanilov/soundscape-qt) is an open-source
system-tray resident desktop application for playing a mix of sounds, e.g.
natural sounds by animals or wind and water.

![](screenshots/overview.png)

## Usage

The application comes with two example soundscapes. Use mouse right-click in the
main window or on the tray icon to access application menu where you can add or
remove soundtracks and change their settings.

On Linux and Windows, use the `Quit` item from the application menu to finish
the application. Pressing the close window button will just minimize the
application to the tray.

[Freesound](https://freesound.org/) is a good source of sounds for your own
soundscapes.

## Installation

Prebuilt binaries for Windows, macOS and Linux are available from the
[Releases page](https://github.com/ddanilov/soundscape-qt/releases).

On Linux you may need to install additional packages in order to run the
application. Most likely XCB util-cursor module is missing. On Debian-based
systems you can install it with

    sudo apt-get install libxcb-cursor0

on openSUSE with

    sudo zypper install libxcb-cursor0

and on RedHat-based sytems with

    sudo dnf install xcb-util-cursor

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
