Summary:  	TxtReader(文本阅读器) for linux
Name:           txtreader
Version:        0.6.5
Release:        1
License:       	GPL
Group:          Desktop/KDE
BuildRoot:     	/var/tmp/%{name}-%{version}-root
Source:        %{name}-%{version}.tar.gz
%description
TxtReader(文本阅读器) for linux 0.6.5
%prep
%setup
%Build
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/share/txtreader
cp -f txtreader $RPM_BUILD_ROOT/usr/share/txtreader/
cp -f icon.png $RPM_BUILD_ROOT/usr/share/txtreader/
cp -f README_*.HTML $RPM_BUILD_ROOT/usr/share/txtreader/
cp -f CHANGES.TXT $RPM_BUILD_ROOT/usr/share/txtreader/
mkdir -p $RPM_BUILD_ROOT/usr/bin
ln -s /usr/share/txtreader/txtreader $RPM_BUILD_ROOT/usr/bin/txtreader
#Install application link for X-Windows
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
cat > cat > $RPM_BUILD_ROOT/usr/share/applications/txtreader.desktop <<EOF
[Desktop Entry]
Name=Txt Reader
GenericName=Txt Reader
GenericName[zh_CN]=文本阅读器
Comment=Txt Reader for linux
Comment[zh_CN]=Txt Reader(文本阅读器) for linux
Exec=/usr/share/txtreader/txtreader
Icon=/usr/share/txtreader/icon.png
Terminal=0
Type=Application
Categories=Qt;KDE;Utility;
EOF
%clean
rm -rf $RPM_BUILD_ROOT
%post
%files
/usr
%changelog
* Mon Mar 1 2011 lsyer <lishao378@sohu.com>
- update to 0.6.4
* Wed Jun 24 2009 lsyer <lishao378@sohu.com>
- create
