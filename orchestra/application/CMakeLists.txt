project (application)

include_directories(screen)

add_executable (
    console_app
    main.cpp
    # Control
    flowcontroller.hpp
    flowcontroller.cpp
    # Common screens
    screen/informationscreen.hpp
    screen/screeniface.hpp
    screen/screencommon.hpp
    screen/screencommon.cpp
    # Login screen
    screen/login/loginscreen.hpp
    screen/login/loginscreen.cpp
    # Back office screen
    screen/backoffice/dashboardscreen.hpp
    screen/backoffice/dashboardscreen.cpp
    screen/backoffice/empmgmtscreen.hpp
    screen/backoffice/empmgmtscreen.cpp
)

set (CONSOLE_APP_LINKER_EXCEPTION "")
if (MINGW)
# This is a temporary solution for now, so we can link with the dlls
set (CONSOLE_APP_LINKER_EXCEPTION "-Wl,-allow-multiple-definition")
endif ()

target_link_libraries (
    console_app
    dashboard
    datamanager
    empmgmt
    loginmod
    ${MINGW_DEPENDENCY}
    ${CONSOLE_APP_LINKER_EXCEPTION}
)
