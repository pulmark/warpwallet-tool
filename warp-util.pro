QT -= gui core

TARGET = warptool

CONFIG += c++14 console
CONFIG -= app_bundle

INCLUDEPATH += \
    $$PWD/include \
    $$PWD/externals/crypto/cppcrypto \
    $$PWD/externals/bitcoin-tool/lib \

DEPENDPATH += \
    $$PWD/externals/bitcoin-tool/lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/WarpKeyGenerator.cc \
    src/CoinKeyPair.cc \
    src/RandomSeedGenerator.cc \
    src/CommandInterpreter.cc \
    src/UserInterface.cc

HEADERS += \
    src/WarpKeyGenerator.h \
    src/CoinKeyPair.h \
    src/RandomSeedGenerator.h \
    src/CommandInterpreter.h \
    src/UserInterface.h

DISTFILES += \
    pwcrack.pro.user

unix:!macx: LIBS += -lcrypto -lssl
#unix:!macx: LIBS += -L$$PWD/externals/fastpbkdf2 -lfastpbkdf2 \
#     -L$$PWD/externals/scrypt/temp -lscrypt \
#     -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool

unix:!macx: LIBS += -L$$PWD/externals/crypto/cppcrypto/ -lcppcrypto \
     -L$$PWD/externals/bitcoin-tool/lib/ -lbitcointool