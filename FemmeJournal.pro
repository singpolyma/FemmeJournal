VERSION = $$system(git describe --always --dirty)

DEFINES += VERSION="\\\"$(shell git describe --always --dirty)\\\""
DEFINES += COMMIT="\\\"$(shell git rev-parse HEAD)\\\""

QMAKE_TARGET_COPYRIGHT = AGPL-3.0+
QMAKE_TARGET_DESCRIPTION = https://FemmeJournal.singpolyma.net
win32 { # On windows version can only be numerical so remove commit hash
	VERSION ~= s/-.*$//
}

PREFIX = $$(PREFIX)
isEmpty(PREFIX):PREFIX = /usr

QT += qml
qtHaveModule(charts): QT += charts
android: QT += svg

CONFIG += c++11

SOURCES += main.cpp \
    calendarmodel.cpp \
    configmodel.cpp \
    cycle.cpp \
    journalentry.cpp \
    qcalparser.cpp \
    statsmodel.cpp \
    symptomsmodel.cpp \
    temperaturechartmodel.cpp

RC_ICONS = icon.ico
ICON = icon.icns
RESOURCES += qml.qrc

lint.target = lint
lint.commands = 'qmllint *.qml'

icon16png.target = icon-16.png
icon16png.commands = rsvg-convert -w 16 -h 16 -o icon-16.png FemmeJournal.svg

icon32png.target = icon-32.png
icon32png.commands = rsvg-convert -w 32 -h 32 -o icon-32.png FemmeJournal.svg

icon36png.target = icon-36.png
icon36png.commands = rsvg-convert -w 36 -h 36 -o icon-36.png FemmeJournal.svg

icon48png.target = icon-48.png
icon48png.commands = rsvg-convert -w 48 -h 48 -o icon-48.png FemmeJournal.svg

icon72png.target = icon-72.png
icon72png.commands = rsvg-convert -w 72 -h 72 -o icon-72.png FemmeJournal.svg

icon96png.target = icon-96.png
icon96png.commands = rsvg-convert -w 96 -h 96 -o icon-96.png FemmeJournal.svg

icon144png.target = icon-144.png
icon144png.commands = rsvg-convert -w 144 -h 144 -o icon-144.png FemmeJournal.svg

icon192png.target = icon-192.png
icon192png.commands = rsvg-convert -w 192 -h 192 -o icon-192.png FemmeJournal.svg

icon256png.target = icon-256.png
icon256png.commands = rsvg-convert -w 256 -h 256 -o icon-256.png FemmeJournal.svg

ico.target = icon.ico
ico.depends = icon-16.png icon-32.png icon-256.png
ico.commands = convert icon-16.png icon-32.png icon-256.png icon.ico

icns.target = icon.icns
icns.depends = icon-16.png icon-32.png icon-256.png
!macx:icns.commands = png2icns icon.icns icon-16.png icon-32.png icon-256.png
macx:icns.commands = \
	mkdir icon.iconset && \
	cp icon-16.png icon.iconset/icon_16x16.png && \
	cp icon-32.png icon.iconset/icon_16x16@2x.png && \
	cp icon-32.png icon.iconset/icon_32x32.png && \
	cp icon-256.png icon.iconset/icon_128x128@2x.png && \
	cp icon-256.png icon.iconset/icon_256x256.png && \
	iconutil --convert icns --output icon.icns icon.iconset && \
	rm -r icon.iconset

androidIcon.target = androidIcon
androidIcon.depends = icon-36.png icon-48.png icon-72.png icon-96.png icon-144.png icon-192.png
androidIcon.commands = \
	mkdir -p android/res/mipmap-ldpi && cp icon-36.png android/res/mipmap-ldpi/icon.png && \
	mkdir -p android/res/mipmap-mdpi && cp icon-48.png android/res/mipmap-mdpi/icon.png && \
	mkdir -p android/res/mipmap-hdpi && cp icon-72.png android/res/mipmap-hdpi/icon.png && \
	mkdir -p android/res/mipmap-xhdpi && cp icon-96.png android/res/mipmap-xhdpi/icon.png && \
	mkdir -p android/res/mipmap-xxhdpi && cp icon-144.png android/res/mipmap-xxhdpi/icon.png && \
	mkdir -p android/res/mipmap-xxxhdpi && cp icon-192.png android/res/mipmap-xxxhdpi/icon.png

FemmeJournal.desktop.files = FemmeJournal.desktop
FemmeJournal.desktop.path = $${PREFIX}/share/applications
unix:!android:INSTALLS += FemmeJournal.desktop

FemmeJournal.svg.files = FemmeJournal.svg
FemmeJournal.svg.path = $${PREFIX}/share/icons/hicolor/scalable/apps
unix:!android:INSTALLS += FemmeJournal.svg

appdata.files = net.singpolyma.FemmeJournal.appdata.xml
appdata.path = $${PREFIX}/share/metainfo
unix:!android:INSTALLS += appdata

QMAKE_EXTRA_TARGETS += lint ico icns androidIcon icon16png icon32png icon36png icon48png icon72png icon96png icon144png icon192png icon256png FemmeJournal.desktop FemmeJournal.svg appdata
PRE_TARGETDEPS += lint
win32:PRE_TARGETDEPS += icon.ico
macx:PRE_TARGETDEPS += icon.icns
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
    symptomsmodel.h \
    temperaturechartmodel.h

DISTFILES += \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
