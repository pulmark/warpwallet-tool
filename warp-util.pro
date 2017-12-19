#
# QT project file to build distribution into single package
# 
# If build fails due to missing external libraries run the following
# build commands to build the external libraries:
#
#   1. Build cppcrypto library into externals/crypto/cppcrypto
#      using the Makefile include in it's sub-dir
#
#   2. Build bitcoin-tool library into externals/bitcoin-tool/lib
#      - command: qmake lib_bitcointool.pro | make
#

# Check if the common config file exists
! include( ./common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = app
QT -= qt
QT -= gui
QT -= core

CONFIG += c++14 console thread link_prl
CONFIG -= app_bundle

TARGET = $$OUT_PWD/warptool

INCLUDEPATH = \
    $$PWD/include \
    $$PWD/externals/crypto/cppcrypto \
    $$PWD/externals/bitcoin-tool/lib \
#    $$PWD/externals/openssl/include

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

# cppcrypto lib: create, linkage, dependencies
unix:!macx: QMAKE_PRE_LINK += make -C $$PWD/externals/crypto/cppcrypto/
unix:!macx: LIBS += -L$$PWD/externals/crypto/cppcrypto/ -lcppcrypto
unix:!macx: QMAKE_PRETARGETDEPS += $$PWD/externals/crypto/cppcrypto/libcppcrypto.a

# bitcointool lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointoold
else:unix: LIBS += -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/externals/bitcoin-tool/lib/libbitcointool.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/externals/bitcoin-tool/lib/libbitcointoold.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/externals/bitcoin-tool/lib/bitcointool.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/externals/bitcoin-tool/lib/bitcointoold.lib
else:unix: PRE_TARGETDEPS += $$PWD/externals/bitcoin-tool/lib/libbitcointool.a

# to test new version of openssl
#unix:!macx: LIBS += -lpthread -ldl
#unix:!macx: LIBS += -L$$PWD/externals/openssl/ -lssl -lcrypto

# system libraries
unix:!macx: LIBS += -lcrypto -lssl -lpthread

message($$OUT_PWD)
