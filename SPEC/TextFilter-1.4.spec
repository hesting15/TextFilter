Summary: Quickly filter text
Name: TextFilter
Version: 1.4
Release: 1
License: LGPL-3.0
Group: Applications/Text
BuildRoot: /var/tmp/%{name}-buildroot


%description
Text Filter
* Use fuzzy match to filter text (e.g. 'ore psu' will find 'Lorem Ipsum').
* Press Enter in the Filter field to go to the next filter result.
* Use Ctrl + Left Mouse Click on the line, to copy whole line to clipboard.
* Press Alt + C several times to extend selection and copy multiple lines to clipboard.

%setup -q

%build
/opt/Qt/5.10.0/gcc_64/bin/qmake /home/stepan/cpp/TextFilter/TextFilter.pro -spec linux-g++
/usr/bin/make qmake_all
/usr/bin/make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin

install -s -m 755 TextFilter $RPM_BUILD_ROOT/usr/bin/TextFilter

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

/usr/bin/TextFilter

%changelog
* Fri Dec 15 2017 Stepan Sypliak
* Changed menu to MS Office style
