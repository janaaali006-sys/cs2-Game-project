cmake_minimum_required(VERSION 3.19)
project(TheQuestofNoReturn LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(TheQuestofNoReturn
    WIN32 MACOSX_BUNDLE

    main.cpp

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    player.cpp
    player.h

    riddlegenerator.cpp
    riddlegenerator.h

    avatargenerator.cpp
    avatargenerator.h

    savemanager.cpp
    savemanager.h

    SharedEnums.h

    Game.h
    Game.cpp
    GameHUD.h

    # Riddles
    riddles/Riddle.cpp
    riddles/Riddle.h
    riddles/LogicRiddle.cpp
    riddles/LogicRiddle.h
    riddles/SequenceRiddle.cpp
    riddles/SequenceRiddle.h
    riddles/IdentityRiddle.cpp
    riddles/IdentityRiddle.h
    riddles/RiddleFallbackPool.cpp
    riddles/RiddleFallbackPool.h

    # World
    world/RiddleManager.cpp
    world/RiddleManager.h
    world/RiddleAltar.cpp
    world/RiddleAltar.h

    # Dialogs
    dialogs/RiddleDialog.cpp
    dialogs/RiddleDialog.h
    dialogs/DilemmaDialog.cpp
    dialogs/DilemmaDialog.h
    dialogs/RevelationDialog.cpp
    dialogs/RevelationDialog.h

    # Outcomes
    outcomes/GoodOutcome.cpp
    outcomes/GoodOutcome.h
    outcomes/HintOutcome.cpp
    outcomes/HintOutcome.h
    outcomes/LoseLifeOutcome.cpp
    outcomes/LoseLifeOutcome.h
    game.h game.cpp
    GameHUD.h
    config.h
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
)

target_link_libraries(TheQuestofNoReturn
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS TheQuestofNoReturn
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#qt_generate_deploy_app_script(
#    TARGET TheQuestofNoReturn
#    OUTPUT_SCRIPT deploy_script
#    NO_UNSUPPORTED_PLATFORM_ERROR
#)
#install(SCRIPT ${deploy_script})
