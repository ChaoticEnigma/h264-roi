
## Options

# General Options
OPTION(LIBCHAOS_SHARED                      "Build LibChaos as a shared library"        OFF )
OPTION(IBERTY_DEMANGLE                      "Use libiberty to demangle symbols"         OFF )

# ZParcel Tool
OPTION(LIBCHAOS_ZPARCEL                     "Build ZParcel tool"                        OFF )

# Library Components
OPTION(LIBCHAOS_EXTERN                      "Build LibChaos with External Libraries"    ON  )
OPTION(LIBCHAOS_UI                          "Build LibChaos UI library"                 OFF )
OPTION(LIBCHAOS_TEST                        "Build LibChaos Test suite"                 ON  )
OPTION(LIBCHAOS_UTIL                        "Build LibChaos Utilities"                  OFF )

# Extern Library Support
OPTION(ENABLE_ZPNG                          "Link zlib and libpng and build ZPNG"       OFF )
OPTION(ENABLE_ZJPEG                         "Link libjepg-turbo and build ZJPEG"        OFF )
OPTION(ENABLE_ZWEBP                         "Link libwebp and build ZWebP"              OFF )
OPTION(ENABLE_ZH264                         "Link libavcodec and libx264 and build ZH264Decoder and ZH264Encoder" OFF)

# Util Executables
OPTION(LIBCHAOS_UTIL_EXTENSION_FIXER        "Build LibChaos Extension Fixer"            OFF )
OPTION(LIBCHAOS_UTIL_AUDIOBOOK_SORTER       "Build LibChaos Audiobook Sorter"           OFF )
OPTION(LIBCHAOS_UTIL_DUPLICATE_FINDER       "Build LibChaos Duplicate Finder"           OFF )
OPTION(LIBCHAOS_UTIL_IMAGE_DOWNLOADER       "Build LibChaos Image Downloader"           OFF )
OPTION(LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER  "Build LibChaos Image Library Manager"      OFF )
OPTION(LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH   "Build LibChaos Image Reverse Search"       OFF )

# All Extra Targets Excluded from All
SET(LIBCHAOS_EXTERN_ALL     EXCLUDE_FROM_ALL)
SET(LIBCHAOS_UI_ALL         EXCLUDE_FROM_ALL)
SET(LIBCHAOS_TEST_ALL       EXCLUDE_FROM_ALL)
SET(LIBCHAOS_UTIL_ALL       EXCLUDE_FROM_ALL)
SET(LIBCHAOS_ZPARCEL_ALL    EXCLUDE_FROM_ALL)
