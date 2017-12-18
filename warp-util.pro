#
# QT project file to build distribution into single package
# 
# Prebuild steps:
#   1. Build cppcrypto library into externals/crypto/cppcrypto
#   2. Build bitcoin-tool library into externals/bitcoin-tool/lib
#

# Check if the common config file exists
! include( ./common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = app
QT -= qt
QT -= gui
QT -= core

CONFIG += c++14 console thread 
CONFIG -= app_bundle

TARGET = $$OUT_PWD/warptool

INCLUDEPATH = \
    $$PWD/include \
    $$PWD/externals/crypto/cppcrypto \
    $$PWD/externals/bitcoin-tool/lib \
//    $$PWD/externals/openssl/include

SOURCES = \
    $$PWD/src/main.cpp \
    $$PWD/src/WarpKeyGenerator.cc \
    $$PWD/src/CoinKeyPair.cc \
    $$PWD/src/RandomSeedGenerator.cc \
    $$PWD/src/CommandInterpreter.cc \
    $$PWD/src/UserInterface.cc

HEADERS = \
    $$PWD/src/WarpKeyGenerator.h \
    $$PWD/src/CoinKeyPair.h \
    $$PWD/src/RandomSeedGenerator.h \
    $$PWD/src/CommandInterpreter.h \
    $$PWD/src/UserInterface.h

# rebuild cppcrypto if changed
unix:!macx: QMAKE_PRE_LINK += make -C $$PWD/externals/crypto/cppcrypto/ -f Makefile

# cppcrypto and bitcointool libraries
unix:!macx: LIBS += -L$$PWD/externals/crypto/cppcrypto/ -lcppcrypto
unix:!macx: LIBS += -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool

# to test newer versions of openssl
#unix:!macx: LIBS += -lpthread -ldl
#unix:!macx: LIBS += -L$$PWD/externals/openssl/ -lssl -lcrypto

# system libraries
unix:!macx: LIBS += -lcrypto -lssl -lpthread

message($$OUT_PWD)
