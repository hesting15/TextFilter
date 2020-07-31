#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T11:08:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextFilter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    Document.cpp \
    FileManager.cpp \
    MainWindow.cpp \
    PlainTextEdit.cpp \
    Settings.cpp \
    SettingsWindow.cpp \
    main.cpp

HEADERS += \
    Document.h \
    FileManager.h \
    MainWindow.h \
    PlainTextEdit.h \
    Settings.h \
    SettingsWindow.h

FORMS += \
    MainWindow.ui \
    SettingsWindow.ui

RC_ICONS += Icon64.ico

DISTFILES += \
    README.md \
    icons/MainTheme/index.theme \
    icons/MainTheme/128x128/copy.png \
    icons/MainTheme/128x128/filter.png \
    icons/MainTheme/128x128/help.png \
    icons/MainTheme/128x128/menu.png \
    icons/MainTheme/128x128/menu_changed.png \
    icons/MainTheme/128x128/multiple_copy.png \
    icons/MainTheme/128x128/new_file - Copy.png \
    icons/MainTheme/128x128/new_file.png \
    icons/MainTheme/128x128/on_top.png \
    icons/MainTheme/128x128/open.png \
    icons/MainTheme/128x128/save.png \
    icons/MainTheme/128x128/save_as.png \
    icons/MainTheme/128x128/save_changed.png \
    icons/MainTheme/128x128/settings.png \
    icons/MainTheme/128x128/wrap_text.png \
    icons/MainTheme/24x24/copy.png \
    icons/MainTheme/24x24/filter.png \
    icons/MainTheme/24x24/help.png \
    icons/MainTheme/24x24/menu.png \
    icons/MainTheme/24x24/menu_changed.png \
    icons/MainTheme/24x24/multiple_copy.png \
    icons/MainTheme/24x24/new_file - Copy.png \
    icons/MainTheme/24x24/new_file.png \
    icons/MainTheme/24x24/on_top.png \
    icons/MainTheme/24x24/open.png \
    icons/MainTheme/24x24/save.png \
    icons/MainTheme/24x24/save_as.png \
    icons/MainTheme/24x24/save_changed.png \
    icons/MainTheme/24x24/settings.png \
    icons/MainTheme/24x24/wrap_text.png \
    icons/MainTheme/32x32/copy.png \
    icons/MainTheme/32x32/filter.png \
    icons/MainTheme/32x32/help.png \
    icons/MainTheme/32x32/menu.png \
    icons/MainTheme/32x32/menu_changed.png \
    icons/MainTheme/32x32/multiple_copy.png \
    icons/MainTheme/32x32/new_file - Copy.png \
    icons/MainTheme/32x32/new_file.png \
    icons/MainTheme/32x32/on_top.png \
    icons/MainTheme/32x32/open.png \
    icons/MainTheme/32x32/save.png \
    icons/MainTheme/32x32/save_as.png \
    icons/MainTheme/32x32/save_changed.png \
    icons/MainTheme/32x32/settings.png \
    icons/MainTheme/32x32/wrap_text.png \
    icons/MainTheme/64x64/copy.png \
    icons/MainTheme/64x64/filter.png \
    icons/MainTheme/64x64/help.png \
    icons/MainTheme/64x64/menu.png \
    icons/MainTheme/64x64/menu_changed.png \
    icons/MainTheme/64x64/multiple_copy.png \
    icons/MainTheme/64x64/new_file - Copy.png \
    icons/MainTheme/64x64/new_file.png \
    icons/MainTheme/64x64/on_top.png \
    icons/MainTheme/64x64/open.png \
    icons/MainTheme/64x64/save.png \
    icons/MainTheme/64x64/save_as.png \
    icons/MainTheme/64x64/save_changed.png \
    icons/MainTheme/64x64/settings.png \
    icons/MainTheme/64x64/wrap_text.png

RESOURCES += \
    resources.qrc
