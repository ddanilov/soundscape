# Soundscape

[![GitHub release](https://img.shields.io/github/release/ddanilov/soundscape)](https://github.com/ddanilov/soundscape/releases/)
[![Coverage Status](https://coveralls.io/repos/github/ddanilov/soundscape/badge.svg)](https://coveralls.io/github/ddanilov/soundscape)

[**Soundscape**](https://github.com/ddanilov/soundscape) is an open-source
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

Command line options:

* `--load <path to file>` load track list from a file on start,
* `--minimize` minimize window to tray on start,
* `--disable-tray` disable tray icon.

[Freesound](https://freesound.org/) is a good source of sounds for your own
soundscapes.

## Installation

### Linux distributions

Packages for some Linux distributions are available.

**Ubuntu** users can install the application from PPA repository
`ppa:ddanilov/soundscape`, see
<https://launchpad.net/~ddanilov/+archive/ubuntu/soundscape> for
details.

**Debian**, **openSUSE** and **Fedora** packages can be installed from
download page of `home:danilov:soundscape` OBS project, see
<https://software.opensuse.org/download/package?package=soundscape&project=home:danilov:soundscape>.

**Flatpak** package is available from **Flathub** app store at
<https://flathub.org/apps/io.github.ddanilov.soundscape>

### Windows

You can install the application using **Windows Package Manager**

    winget search soundscape
    winget install Danilov.Soundscape

### Prebuilt binaries

Prebuilt binaries for Windows, macOS and Linux are available from the
[Releases page](https://github.com/ddanilov/soundscape/releases).

On Linux you may need to install additional packages in order to run the
prebuilt binary. Most likely XCB util-cursor module is missing. On Debian-based
systems you can install it with

    sudo apt-get install libxcb-cursor0

on openSUSE with

    sudo zypper install libxcb-cursor0

and on RedHat-based systems with

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
