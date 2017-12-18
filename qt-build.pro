#
# Main build file for warpwallet-tool executables and libraries
#

# Check if the common config file exists
! include( ./common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = subdirs

# where to find the sub projects and their QT project build file names

lib-warpwallet.file = logic/lib_warpwallet.pro
lib-bitcointool.file = externals/bitcoin-tool/lib/lib_bitcointool.pro
lib-crypto.file = crypto/lib_cppcrypto.pro
app-tool.file = tool/tool.pro
#app-test.file = test/test.pro

# what subproject depends on others

lib-warpwallet.depends = lib-crypto lib-bitcointool
app-tool.depends = lib-warpwallet
#app-test.depends = lib-warpwallet

SUBDIRS += \
    lib-bitcointool \
    lib-crypto \
    lib-warpwallet \
    app-tool \
#    app-test

# build the project sequentially as listed in SUBDIRS !
# CONFIG += ordered

message($$OUT_PWD)
   