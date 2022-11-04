# Find all source files in the current directory
# and save the name to the DIR_examples_SRCS variable
aux_source_directory(. DIR_SD_SRCS)

include_directories(../lib/config)
include_directories(../lib/lcd)
include_directories(../lib/font)
include_directories(../lib/fatfs)
include_directories(../lib/sdcard)
include_directories(../sd)

# Build the linked library
add_library(sd ${DIR_SD_SRCS})
target_link_libraries(sd PUBLIC examples config lcd font fatfs sdcard hardware_adc pico_float)
