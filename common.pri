#
# Includes common configuration for all subdirectory .pro files.
#

INCLUDEPATH += . .. \
    ../include \
    ../externals/crypto/cppcrypto \
    ../externals/bitcoin-tool/lib

WARNINGS += -Wall

# keep the generated files separated from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs