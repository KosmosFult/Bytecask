aux_source_directory(. DIR_LIB_SRCS)
add_executable(cmd ${DIR_LIB_SRCS})
include_directories(
    ${PROJECT_SOURCE_DIR}/src/libs/inifile-cpp
)