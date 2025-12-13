
set(CXXRT_SOURCES
    dynamic_cast.cc
    guard.cc
    stdexcept.cc
    typeinfo.cc
    memory.cc
    auxhelper.cc
    libelftc_dem_gnu3.c
    cxa_atexit.cc
    cxa_thread_atexit.cc
    cxa_vectors.cc
   )

if (NOT CXXRT_NO_EXCEPTIONS)
set(CXXRT_SOURCES
    ${CXXRT_SOURCES}
    exception.cc
   )
else()
set(CXXRT_SOURCES
    ${CXXRT_SOURCES}
    noexception.cc
   )
endif()

add_library(cxxrt-static STATIC ${CXXRT_SOURCES})
add_library(cxxrt-shared SHARED ${CXXRT_SOURCES})
target_add_definitions(cxxrt-static PRIVATE -D_GNU_SOURCE)
target_add_definitions(cxxrt-shared PRIVATE -D_GNU_SOURCE)
target_compile_options(cxxrt-shared PRIVATE -O3 -DNDEBUG -fvisibility=hidden -fvisibility-inlines-hidden -D_GNU_SOURCE)
if (CXXRT_NO_EXCEPTIONS)
    target_add_definitions(cxxrt-static PRIVATE -DLIBCXXRT_NO_EXCEPTIONS)
    target_add_definitions(cxxrt-shared PRIVATE -DLIBCXXRT_NO_EXCEPTIONS)
endif()

target_link_libraries(cxxrt-shared ${CMAKE_DL_LIBS})
target_link_libraries(cxxrt-static ${CMAKE_DL_LIBS})

set_target_properties(cxxrt-static cxxrt-shared PROPERTIES
                      OUTPUT_NAME "cxxrt"
                     )

