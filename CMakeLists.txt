cmake_minimum_required (VERSION 3.2)
project (MidiSetup VERSION 1.0.0 LANGUAGES C)

if(NOT WIN32)
    message(FATAL_ERROR "MidiSetup: This component is for Windows Platform only")
endif()


#====================================
#       Control panel applet
#====================================

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCHITECTURE_BITS "86")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE_BITS "64")
else()
    set(ARCHITECTURE_BITS "unk")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/x${ARCHITECTURE_BITS})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/x${ARCHITECTURE_BITS})


set(MIDISETUP_SRC
    src/midiselect.rc
    src/config_dialog.c
)

set(MIDISETUP_CPL_LIBS winmm comctl32 gdi32 user32)

add_library(midiselect MODULE
    ${MIDISETUP_SRC}
    src/midiselect.def
    src/midiselect-cpl.c
    src/res-cpl.rc
)
set_target_properties(midiselect PROPERTIES PREFIX "" OUTPUT_NAME "midiselect" SUFFIX ".cpl")
target_compile_definitions(midiselect PRIVATE
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
    -D_CRT_SECURE_NO_WARNINGS
    -D_UNICODE
    -DUNICODE
    -DADL_IS_CPL_APPLET
)

target_link_libraries(midiselect PRIVATE ${MIDISETUP_CPL_LIBS} -static-libgcc -Wl,--enable-stdcall-fixup)


add_executable(midiselecttool
    ${MIDISETUP_SRC}
    src/midiselect-tool.c
    src/res-tool.rc
)
set_target_properties(midiselecttool PROPERTIES WIN32_EXECUTABLE ON)
target_compile_definitions(midiselecttool PRIVATE
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
    -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
    -D_CRT_SECURE_NO_WARNINGS
    -D_UNICODE
    -DUNICODE
)

target_link_libraries(midiselecttool PRIVATE ${MIDISETUP_CPL_LIBS} -static-libgcc)
