add_library(libs INTERFACE)

#flecs

add_subdirectory(flecs)
target_link_libraries(libs INTERFACE flecs::flecs_static)


#box2d
add_subdirectory(box2d)
set(BOX2D_BUILD_TESTBED OFF)
target_link_libraries(libs INTERFACE box2d)

#box2d
add_subdirectory(json)
target_link_libraries(libs INTERFACE nlohmann_json::nlohmann_json)

#glm
#add_subdirectory(glm)
#target_link_libraries(libs INTERFACE glm)

#bixxy-engine
add_subdirectory(bixxy-engine)
target_link_libraries(libs INTERFACE bixxy-engine)
target_include_directories(libs INTERFACE bixxy-engine)