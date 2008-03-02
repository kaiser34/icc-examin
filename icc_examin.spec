Summary: ICC profile viewer and colour visualisation
Name: icc_examin
Version: 0.36
Release: 1
License: GPL,LGPL
Group: Productivity/Graphics/Other
Source: icc_examin_0.36.tar.gz
#Source: http://www.behrmann.name/index.php?option=com_content&task=view&id=33&Itemid=69
URL: http://www.behrmann.name
Vendor: Kai-Uwe Behrmann
Packager: Kai-Uwe Behrmann <ku.b@gmx.de>
BuildRoot: %{_tmppath}/%{name}-root
Prefix:    %{_prefix}

%description
ICC Examin is a small utility (unix name: icc_examin) for the
purpos of watching the internals of ICC profiles, measurement
data (CGATS), argylls gamut visualisations (vrml) and
video card gamma tables (Xorg/XFree86/osX).


Authors:
--------
    ICC Examin  Kai-Uwe Behrmann <www.behrmann.name>

used components and libraries (partitial statically):
    agv         Philip Winston
    freeglut    Pawel W. Olszta
    littleCMS   Marti Maria
    FLTK        Bill Spizak and others
    FLU         Jason Bryan
    FTGL        Henry Maddocks
    Oyranos     Kai-Uwe Behrmann
    Elektra     Avi Alkalay and others

%prep
%setup -q -n %{name}_%{version}
#-%{release}

%build
%configure
configure --prefix %{_prefix}
make %{_smp_mflags} static

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

%clean
rm -rf %{buildroot}

%post
update-mime-database %{_datadir}/mime/

%postun
update-mime-database %{_datadir}/mime/

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog README
%{_bindir}/icc_examin
%{_datadir}/applications/icc_examin.desktop
%{_datadir}/mime/packages/icc.xml
%{_datadir}/pixmaps/icc_examin.png
%{_datadir}/fonts/FreeSans.ttf
%{_datadir}/locale/de/LC_MESSAGES/icc_examin.mo


%changelog
* Fri Aug 12 2005 Kai-Uwe Behrmann
- GUI improvements

* Tue May 26 2005 Kai-Uwe Behrmann
- GUI improvements
- translation to german in v0.30

* Tue Apr 21 2005 Kai-Uwe Behrmann
- first release


