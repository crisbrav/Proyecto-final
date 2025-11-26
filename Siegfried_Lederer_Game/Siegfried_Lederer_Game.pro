QT += widgets gui core multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    baselevel.cpp \
    bomb.cpp \
    explosion.cpp \
    gamemanager.cpp \
    gameobject.cpp \
    guard.cpp \
    level1.cpp \
    level2.cpp \
    level3.cpp \
    main.cpp \
    mainwindow.cpp \
    mazegrid.cpp \
    movingtrap.cpp \
    player.cpp \
    projectile.cpp

HEADERS += \
    baselevel.h \
    bomb.h \
    explosion.h \
    gamemanager.h \
    gameobject.h \
    guard.h \
    level1.h \
    level2.h \
    level3.h \
    mainwindow.h \
    mazegrid.h \
    movingtrap.h \
    player.h \
    projectile.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets.qrc

DISTFILES += \
    ../../../../Desktop/recursos proyecto final/363_full_game-of-rings_0155_preview.mp3 \
    ../../../../Desktop/recursos proyecto final/drop_bomb.png \
    ../../../../Desktop/recursos proyecto final/prisoner_level3.png
