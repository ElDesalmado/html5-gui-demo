
add_executable(c60
        c60-main.cpp
		c60-main_app.cpp
		c60-main_app.h
        c60-browser_manager.cpp
        c60-browser_manager.h
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(c60
    cefdll_wrapper
)

# this should be added by cef-cmake
# but to support earlier cmake versions which don't support
# target_link_options we manually add them here
if (CEF_CMAKE_OS_POSIX)
    set_target_properties(c60t PROPERTIES
        # passing local rpath linker flags
        LINK_FLAGS "-Wl,-rpath,.:$ORIGIN"
    )
endif()

#add_dependencies(c60 HtmlCopyTarget)