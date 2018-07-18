zypper install gcc-c++
zypper install gcc
zypper install glibc-devel-32bit


sudo find . | grep libGL
sudo ln -s /usr/lib/x86_64-linux-gnu/mesa/libGL.so.1 /usr/lib/libGL.sosudo apt-get install libgl1-mesa-dev
sudo zypper install --type pattern devel_basis

PATH=$PATH:/opt/Qt/5.10.0/gcc_64/bin/

wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage

chmod a+x linuxdeployqt-continuous-x86_64.AppImage

./linuxdeployqt-continuous-x86_64.AppImage ../TextFilter/SPEC/AppDir/usr/share/applications/default.desktop -appimage

