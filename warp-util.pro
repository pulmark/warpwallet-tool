TEMPLATE = app
QT -= qt

CONFIG += c++14 console thread debug_and_release
CONFIG -= app_bundle

TARGET = warptool

INCLUDEPATH = \
    $$PWD/include \
    $$PWD/externals/crypto/cppcrypto \
    $$PWD/externals/bitcoin-tool/lib \
//    $$PWD/externals/openssl/include

DEPENDPATH = \
    $$PWD/externals/bitcoin-tool/lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0

SOURCES = \
    src/main.cpp \
    src/WarpKeyGenerator.cc \
    src/CoinKeyPair.cc \
    src/RandomSeedGenerator.cc \
    src/CommandInterpreter.cc \
    src/UserInterface.cc

HEADERS = \
    src/WarpKeyGenerator.h \
    src/CoinKeyPair.h \
    src/RandomSeedGenerator.h \
    src/CommandInterpreter.h \
    src/UserInterface.h

DISTFILES = \
    warp-util.pro.user

unix:!macx: LIBS += -L$$PWD/externals/crypto/cppcrypto/ -lcppcrypto
unix:!macx: LIBS += -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool
#unix:!macx: LIBS += -lpthread -ldl
#unix:!macx: LIBS += -L$$PWD/externals/openssl/ -lssl -lcrypto
unix:!macx: LIBS += -lcrypto -lssl -lpthread
