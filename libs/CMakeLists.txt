add_library(libs INTERFACE)

#flecs

add_subdirectory(flecs)
target_link_libraries(libs INTERFACE flecs::flecs_static)


#box2d
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Disable box2d test")
add_subdirectory(box2d)
target_link_libraries(libs INTERFACE box2d)

#box2d
add_subdirectory(json)
target_link_libraries(libs INTERFACE nlohmann_json::nlohmann_json)

#glm
#add_subdirectory(glm)
#target_link_libraries(libs INTERFACE glm)

#bixxy-engine

set(ADD_IMGUI ${ADD_IMGUI} CACHE BOOL "Enable ImGui support" FORCE)

add_subdirectory(bixxy-engine)
target_link_libraries(libs INTERFACE bixxy-engine)
target_include_directories(libs INTERFACE bixxy-engine)