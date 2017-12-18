#
# QT project to build cppcrypto static library.
#

TEMPLATE = lib
QT -= qt
QT -= core
QT -= gui

# dummy target (for some reason this is needed)
TARGET = $$PWD/dummy

CONFIG += staticlib c++14 thread

# QT magic: uses the original Makefile
mytarget.target =
mytarget.commands = make -C $$PWD/../externals/crypto/cppcrypto/
mytarget.depends = dummy
dummy.commands = @echo Building $$mytarget.target
QMAKE_EXTRA_TARGETS += mytarget dummy
PRE_TARGETDEPS += mytarget dummy

# copies the library into build directory
QMAKE_POST_LINK += cp $$PWD/../externals/crypto/cppcrypto/libcppcrypto.a $$OUT_PWD/

# old brute force implementation
#QMAKE_EXTRA_TARGETS += lib_cppcrypto
#PRE_TARGETDEPS += lib_cppcrypto
#
# compile and copy static library file from externals sub-dir
#lib_cppcrypto.commands = \
#    rm -f $$OUT_PWD/libcppcrypto.a >/dev/null | \
#    make -C $$PWD/../externals/crypto/cppcrypto/ -f Makefile >/dev/null | \
#    cp $$PWD/../externals/crypto/cppcrypto/libcppcrypto.a $$OUT_PWD/libcppcrypto.a >/dev/null \

message($$OUT_PWD)
