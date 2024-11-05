QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    main.cpp \
    replacedialog.cpp \
    searchdialog.cpp \
    texteditor.cpp

HEADERS += \
    aboutdialog.h \
    replacedialog.h \
    searchdialog.h \
    texteditor.h

FORMS += \
    aboutdialog.ui \
    replacedialog.ui \
    searchdialog.ui \
    texteditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/add_circle_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/content_copy_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/content_cut_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/content_paste_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/file_open_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/format_color_text_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/palette_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/redo_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/save_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png \
    images/undo_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.png

RESOURCES += \
    1.qrc
