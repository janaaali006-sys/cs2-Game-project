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

    # Person 5 — Player, AI generators, Save
    player.cpp
    player.h
    riddlegenerator.cpp
    riddlegenerator.h
    avatargenerator.cpp
    avatargenerator.h
    savemanager.cpp
    savemanager.h
    config.h

    # Person 5 — OOP base classes
    SharedEnums.h
    GameObject.h
    GameObject.cpp
    Artefact.h
    Artefact.cpp

    # Person 1 — Game engine, rooms, sprites
    Game.h
    Game.cpp
    Room.h
    Room.cpp
    RoomManager.h
    RoomManager.cpp
    PlayerSprite.h
    PlayerSprite.cpp
    DoorItem.h
    DoorItem.cpp
    TrapItem.h
    TrapItem.cpp

    # Person 2 — Riddles
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

    # Person 2 — World objects
    world/RiddleManager.cpp
    world/RiddleManager.h
    world/RiddleAltar.cpp
    world/RiddleAltar.h

    # Person 2 — Dialogs
    dialogs/RiddleDialog.cpp
    dialogs/RiddleDialog.h
    dialogs/DilemmaDialog.cpp
    dialogs/DilemmaDialog.h
    dialogs/RevelationDialog.cpp
    dialogs/RevelationDialog.h

    # Person 2 — Outcomes
    outcomes/GoodOutcome.cpp
    outcomes/GoodOutcome.h
    outcomes/HintOutcome.cpp
    outcomes/HintOutcome.h
    outcomes/LoseLifeOutcome.cpp
    outcomes/LoseLifeOutcome.h
    GameObject.cpp GameObject.h
    avatargenerator.cpp avatargenerator.h CMakeLists.txt config.h DoorItem.cpp DoorItem.h Game.cpp Game.h GameHUD.h GameObject.cpp GameObject.h mainwindow.cpp mainwindow.h player.cpp player.h PlayerSprite.cpp PlayerSprite.h riddlegenerator.cpp riddlegenerator.h Room.cpp Room.h RoomManager.cpp RoomManager.h savemanager.cpp savemanager.h SharedEnums.h TrapItem.cpp TrapItem.h background.png characterPage.png Karim.png main.cpp mainwindow.ui manuscript.png NOOR.png RAMI.png resources.qrc resources.qrc.autosave scholarIcon.png speedsterIcon.png storyBackground.png survivorIcon.png Zara.png
    GameObject.cpp GameObject.h
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
