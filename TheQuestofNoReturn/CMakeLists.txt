cmake_minimum_required(VERSION 3.19)
project(TheQuestofNoReturn LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(TheQuestofNoReturn
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    pausemenudialog.h pausemenudialog.cpp pausemenudialog.ui
    difficultydialog.h difficultydialog.cpp difficultydialog.ui
    SharedEnums.h

)

qt_add_resources(TheQuestofNoReturn "resources"
    PREFIX "/images"
    FILES
        background.png
	characterPage.png
	Zara.png
	Karim.png
        RAMI.png
	NOOR.png
        scholarIcon.png
        survivorIcon.png
        speedsterIcon.png
        storyBackground.png
        gameBackground.png
)

target_link_libraries(TheQuestofNoReturn
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS TheQuestofNoReturn
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TheQuestofNoReturn
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
