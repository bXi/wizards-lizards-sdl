add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        components/aicontroller.h
        components/collision.h
        components/damage.h
        components/health.h
        components/owner.h
        components/playerclass.cpp
        components/playerclass.h
        components/playerindex.h
        components/playerinput.h
        components/radius.h
        components/render2d.cpp
        components/render2d.h
        components/rigidbody2d.h
        components/sprite.h
        components/type.h
        components/weapon.h
        ecs.h
        entities/classes/baseclass.h
        entities/classes/berserker.h
        entities/classes/bouncer.h
        entities/classes/trapper.h
        entities/classes/wizard.cpp
        entities/classes/wizard.h
        entities/fireball.cpp
        entities/fireball.h
        entities/lizard.cpp
        entities/lizard.h
        entities/physicsobject.h
        entities/player.cpp
        entities/player.h
        entities/spawner.h
        main.cpp
        main.h
        map/dungeon.h
        map/levelmanager.cpp
        map/levelmanager.h
        map/newdungeon.cpp
        map/newdungeon.h
        map/room.h
        states/game/game.cpp
        states/game/game.h
#        states/intro/intro.cpp
#        states/intro/intro.h
        states/menu/menu.cpp
        states/menu/menu.h
        states/menu/menustructure.h
        states/test/box2dobjects.cpp
        states/test/box2dobjects.h
        states/test/test.cpp
        states/test/test.h
        ui/ui.cpp
        ui/ui.h
        world/world.cpp
        world/world.h
)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    if (EMSCRIPTEN)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-g -s FORCE_FILESYSTEM=1 -s TOTAL_MEMORY=67108864 -sUSE_SDL=3 -sUSE_SDL_MIXER=3 -sSTACK_SIZE=5MB -s DEMANGLE_SUPPORT=1 -s EXPORTED_FUNCTIONS=\"[_malloc,_main]\" --shell-file ${CMAKE_SOURCE_DIR}/assets/shell.html --preload-file ${CMAKE_SOURCE_DIR}/assets/@assets --bind")
        set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so taht you can directly open it.

        set(OUTPUT_DIR "../../../bin/Emscripten")
    else()
        set(OUTPUT_DIR "../../../bin/Release")
    endif()

else()
    set(OUTPUT_DIR "../../../bin/Debug")
endif()


if (NOT EMSCRIPTEN)
    file(COPY ../assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../../../bin/${CMAKE_BUILD_TYPE})
endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} PRIVATE libs)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-command-line-argument)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)