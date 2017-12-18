#
# QT project to build cppcrypto static library.
#

QMAKE_EXTRA_TARGETS += lib_cppcrypto
PRE_TARGETDEPS += lib_cppcrypto

# compile and copy static library file from externals sub-dir
lib_cppcrypto.commands = \
    rm -f $$OUT_PWD/libcppcrypto.a >/dev/null | \
    make -C $$PWD/../externals/crypto/cppcrypto/ -f Makefile >/dev/null | \
    cp $$PWD/../externals/crypto/cppcrypto/libcppcrypto.a $$OUT_PWD/libcppcrypto.a >/dev/null \

message($$OUT_PWD)
   