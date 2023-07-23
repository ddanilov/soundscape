Name:           soundscape
Version:        1.3.1
Release:        1
Summary:        Desktop soundscape application

License:        GPL-3.0-only
Source:         %{name}_%{version}.orig.tar.gz

BuildRequires:  cmake
BuildRequires:  desktop-file-utils
BuildRequires:  gcc-c++
BuildRequires:  hicolor-icon-theme
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt6Core)
BuildRequires:  pkgconfig(Qt6Multimedia)
BuildRequires:  pkgconfig(Qt6Test)
BuildRequires:  pkgconfig(Qt6Widgets)

%description
Soundscape is an open-source system-tray resident desktop application
for playing a mix of sounds, e.g. natural sounds by animals or wind
and water.

%prep
%autosetup

%build
%cmake -DCMAKE_SKIP_RPATH=ON -DAPP_VERSION=%{version} -DCMAKE_INSTALL_DOCDIR=%{_docdir}/%{name}
%cmake_build

%install
%cmake_install
desktop-file-validate %{buildroot}/%{_datadir}/applications/soundscape.desktop

%files
%{_bindir}/%{name}
%{_datadir}/%{name}/
%{_datadir}/applications/soundscape.desktop
%{_datadir}/icons/hicolor/scalable/apps/soundscape.svg
%{_docdir}/%{name}/

%changelog
%autochangelog
