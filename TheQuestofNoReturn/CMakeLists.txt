cmake_minimum_required(VERSION 3.19)
project(TheQuestofNoReturn LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(TheQuestofNoReturn
    WIN32 MACOSX_BUNDLE

    main.cpp

    # ── UI / MainWindow ──────────────────────────────
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    # ── Person 4 — Dialogs / Screens ────────────────
    pausemenudialog.h pausemenudialog.cpp pausemenudialog.ui
    difficultydialog.h difficultydialog.cpp difficultydialog.ui
    howtoplaydialog.h howtoplaydialog.cpp howtoplaydialog.ui

    # ── Person 5 — Player, AI, Save ─────────────────
    player.cpp player.h
    riddlegenerator.cpp riddlegenerator.h
    avatargenerator.cpp avatargenerator.h
    savemanager.cpp savemanager.h
    config.h AIgenerator.h

    # ── Person 5 — OOP base classes ─────────────────
    SharedEnums.h
    GameObject.h GameObject.cpp
    Artefact.h Artefact.cpp

    # ── Person 1 — Game engine ───────────────────────
    Game.h Game.cpp
    Room.h Room.cpp
    RoomManager.h RoomManager.cpp
    PlayerSprite.h PlayerSprite.cpp
    DoorItem.h DoorItem.cpp
    TrapItem.h TrapItem.cpp

    # ── Person 2 — Riddles ───────────────────────────
    riddles/Riddle.cpp riddles/Riddle.h
    riddles/LogicRiddle.cpp riddles/LogicRiddle.h
    riddles/SequenceRiddle.cpp riddles/SequenceRiddle.h
    riddles/IdentityRiddle.cpp riddles/IdentityRiddle.h
    riddles/RiddleFallbackPool.cpp riddles/RiddleFallbackPool.h

    # ── Person 2 — World / Dialogs / Outcomes ────────
    world/RiddleManager.cpp world/RiddleManager.h
    world/RiddleAltar.cpp world/RiddleAltar.h
    dialogs/RiddleDialog.cpp dialogs/RiddleDialog.h
    dialogs/DilemmaDialog.cpp dialogs/DilemmaDialog.h
    dialogs/RevelationDialog.cpp dialogs/RevelationDialog.h
    outcomes/GoodOutcome.cpp outcomes/GoodOutcome.h
    outcomes/HintOutcome.cpp outcomes/HintOutcome.h
    outcomes/LoseLifeOutcome.cpp outcomes/LoseLifeOutcome.h

    # ── Person 3 — Guardians / Encounters (stubs) ────
    Guardiansprite.h Guardiansprite.cpp
    watcherguardian.h watcherguardian.cpp
    pursuerguardian.h pursuerguardian.cpp
    riddlerguardian.h riddlerguardian.cpp
    watcherdialog.h watcherdialog.cpp
    pursuerdialog.h pursuerdialog.cpp
    bossbattle.h bossbattle.cpp
    encountermanager.h encountermanager.cpp
    artefactitem.h artefactitem.cpp
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
        gameover_background.png
        winBackground.png
        ankh.png
        difficulty.png
        manuscript.png
)

target_link_libraries(TheQuestofNoReturn
    PRIVATE Qt::Core Qt::Widgets Qt::Network
)

include(GNUInstallDirs)

install(TARGETS TheQuestofNoReturn
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
