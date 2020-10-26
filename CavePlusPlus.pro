
CONFIG += c++11 console
#CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        move.cpp \
        place.cpp \
        test.cpp \
        tom.cpp \
        cave.cpp \
        location.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    blocks.h \
    cave.h \
    coin.h \
    command.h \
    location.h \
    move.h \
    mushroom.h \
    place.h \
    rock.h \
    thing.h \
    tom.h
