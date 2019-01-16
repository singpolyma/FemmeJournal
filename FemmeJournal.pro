PREFIX = $$(PREFIX)
isEmpty(PREFIX):PREFIX = /usr

QT += svg qml quick

CONFIG += c++11

SOURCES += main.cpp \
    calendarmodel.cpp \
    configmodel.cpp \
    cycle.cpp \
    journalentry.cpp \
    qcalparser.cpp \
    statsmodel.cpp \
    symptomsmodel.cpp

RC_ICONS = icon.ico
ICON = icon.icns
RESOURCES += qml.qrc

lint.target = lint
lint.commands = 'qmllint *.qml'

icon16png.target = icon-16.png
icon16png.commands = inkscape -z -e icon-16.png -w 16 -h 16 FemmeJournal.svg

icon32png.target = icon-32.png
icon32png.commands = inkscape -z -e icon-32.png -w 32 -h 32 FemmeJournal.svg

icon36png.target = icon-36.png
icon36png.commands = inkscape -z -e icon-36.png -w 36 -h 36 FemmeJournal.svg

icon48png.target = icon-48.png
icon48png.commands = inkscape -z -e icon-48.png -w 48 -h 48 FemmeJournal.svg

icon72png.target = icon-72.png
icon72png.commands = inkscape -z -e icon-72.png -w 72 -h 72 FemmeJournal.svg

icon96png.target = icon-96.png
icon96png.commands = inkscape -z -e icon-96.png -w 96 -h 96 FemmeJournal.svg

icon144png.target = icon-144.png
icon144png.commands = inkscape -z -e icon-144.png -w 144 -h 144 FemmeJournal.svg

icon192png.target = icon-192.png
icon192png.commands = inkscape -z -e icon-192.png -w 192 -h 192 FemmeJournal.svg

icon256png.target = icon-256.png
icon256png.commands = inkscape -z -e icon-256.png -w 256 -h 256 FemmeJournal.svg

ico.target = icon.ico
ico.depends = icon-16.png icon-32.png icon-256.png
ico.commands = convert icon-16.png icon-32.png icon-256.png icon.ico

icns.target = icon.icns
icns.depends = icon-16.png icon-32.png icon-256.png
icns.commands = png2icns icon.icns icon-16.png icon-32.png icon-256.png

androidIcon.target = androidIcon
androidIcon.depends = icon-36.png icon-48.png icon-72.png icon-96.png icon-144.png icon-192.png
androidIcon.commands = \
	mkdir -p android/res/mipmap-ldpi && cp icon-36.png android/res/mipmap-ldpi/icon.png && \
	mkdir -p android/res/mipmap-mdpi && cp icon-48.png android/res/mipmap-mdpi/icon.png && \
	mkdir -p android/res/mipmap-hdpi && cp icon-72.png android/res/mipmap-hdpi/icon.png && \
	mkdir -p android/res/mipmap-xhdpi && cp icon-96.png android/res/mipmap-xhdpi/icon.png && \
	mkdir -p android/res/mipmap-xxhdpi && cp icon-144.png android/res/mipmap-xxhdpi/icon.png && \
	mkdir -p android/res/mipmap-xxxhdpi && cp icon-192.png android/res/mipmap-xxxhdpi/icon.png

FemmeJournal.svg.files = FemmeJournal.svg
FemmeJournal.svg.path = $${PREFIX}/share/icons/hicolor/scalable/apps
unix:!android:INSTALLS += FemmeJournal.svg

QMAKE_EXTRA_TARGETS += lint ico icns androidIcon icon16png icon32png icon36png icon48png icon72png icon96png icon144png icon192png icon256png FemmeJournal.svg
PRE_TARGETDEPS += lint
win32:PRE_TARGETDEPS += ico
macx:PRE_TARGETDEPS += icns
android:PRE_TARGETDEPS += androidIcon

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${PREFIX}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    calendarmodel.h \
    configmodel.h \
    cycle.h \
    journalentry.h \
    qcalparser.h \
    statsmodel.h \
    symptomsmodel.h

DISTFILES += \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
