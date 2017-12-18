#
# QT project to build core logic into static library.
#

# Check if the common config file exists
! include( $$PWD/../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = lib
QT -= qt
QT -= core
QT -= gui


TARGET = $$OUT_PWD/warpwallet

CONFIG += staticlib c++14 thread

SOURCES += \
    $$PWD/../src/CoinKeyPair.cc \
    $$PWD/../src/CommandInterpreter.cc \
    $$PWD/../src/RandomSeedGenerator.cc \
    $$PWD/../src/UserInterface.cc \
    $$PWD/../src/WarpKeyGenerator.cc

HEADERS += \
    $$PWD/../src/CoinKeyPair.h \
    $$PWD/../src/CommandInterpreter.h \
    $$PWD/../src/RandomSeedGenerator.h \
    $$PWD/../src/UserInterface.h \
    $$PWD/../src/WarpKeyGenerator.h

# unix:!macx: QMAKE_POST_LINK += make -C $$OUT_PWD/../externals/crypto/cppcrypto -f Makefile

message($$OUT_PWD)
   