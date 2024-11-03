Name:           soundscape
Version:        1.5.0
Release:        1
Summary:        Desktop soundscape application

License:        GPL-3.0-only AND CC-BY-3.0
Source:         %{name}_%{version}.orig.tar.gz

%define is_leap %{defined suse_version} && 0%{?suse_version} < 01600

BuildRequires:  cmake
BuildRequires:  desktop-file-utils
%if %{is_leap}
BuildRequires:  gcc12-c++
%else
BuildRequires:  gcc-c++
%endif
BuildRequires:  hicolor-icon-theme
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt6Core)
BuildRequires:  pkgconfig(Qt6Linguist)
BuildRequires:  pkgconfig(Qt6Multimedia)
BuildRequires:  pkgconfig(Qt6Test)
BuildRequires:  pkgconfig(Qt6Widgets)

%if %{defined suse_version}
Requires:       qt6-multimedia
Requires:       libQt6Svg6
%endif

%if %{defined fedora}
Requires:       qt6-qtsvg
%endif

%description
Soundscape is an open-source system-tray resident desktop application
for playing a mix of sounds, e.g. natural sounds by animals or wind
and water.

%prep
%autosetup

%build
%if %{is_leap}
export CXX=g++-12
%endif
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
%{_datadir}/metainfo/soundscape.metainfo.xml
%{_docdir}/%{name}/

%if %{defined fedora}
%changelog
%autochangelog
%endif
