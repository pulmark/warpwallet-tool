#
# QT project to build warptool executable.
#

# Check if the common config file exists
! include( $$PWD/../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = app
QT -= qt
QT -= gui
QT -= core

CONFIG += c++14 console thread link_prl
CONFIG -= app_bundle

TARGET = $$OUT_PWD/warptool

INCLUDEPATH += \
	$$PWD/../include \
	$$PWD/../src

# libraries
unix:!macx: LIBS += -L$$OUT_PWD/../logic/ -lwarpwallet
unix:!macx: LIBS += -L$$OUT_PWD/../crypto/ -lcppcrypto
unix:!macx: LIBS += -L$$OUT_PWD/../externals/bitcoin-tool/lib/ -lbitcointool
unix:!macx: LIBS += -lcrypto -lssl -lpthread

# dependencies
unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../logic/libwarpwallet.a
unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../crypto/libcppcrypto.a
unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../externals/bitcoin-tool/lib/libbitcointool.a

SOURCES = $$PWD/../src/main.cpp

message($$OUT_PWD)
