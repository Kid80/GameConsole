# Find all source files in the current directory
# and save the name to the DIR_examples_SRCS variable
aux_source_directory(. DIR_EXAMPLES_SRCS)

include_directories(../lib/config)
include_directories(../lib/lcd)
include_directories(../lib/font)
include_directories(../lib/fatfs)
include_directories(../lib/sdcard)
include_directories(../sd)

# Build the linked library
add_library(examples ${DIR_EXAMPLES_SRCS})
target_link_libraries(examples PUBLIC config lcd font fatfs sdcard hardware_adc pico_float)
