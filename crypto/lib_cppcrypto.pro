#
# QT project to build cppcrypto static library using external Makefile
#

TEMPLATE = lib
QT -= qt
QT -= core
QT -= gui

# dummy's target
TARGET = $$PWD/cppcrypto

CONFIG += staticlib

# QT magic, step 1: build real target using phony
mytarget.target = libcppcrypto.a
mytarget.path = $$PWD/../externals/crypto/cppcrypto/libcppcrypto.a
mytarget.commands = make -f phony
mytarget.depends = dummy

# QT magic, step 2: use 'dummy' as extra target for forwarding build reguest to 'mytarget'
dummy.commands = @echo Building $$mytarget.target
QMAKE_EXTRA_TARGETS += mytarget dummy
PRE_TARGETDEPS += dummy

# QT magic, step 3: copy the real library from sub-dir into build dir after linkage
QMAKE_POST_LINK += cp $$mytarget.path $$OUT_PWD/

message($$OUT_PWD)
