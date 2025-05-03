file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(UnitTests ${SRC_FILES})

find_package(Catch2 REQUIRED)

target_link_libraries(UnitTests
    PRIVATE
        voxel_engine
        fastnoise-lite::fastnoise-lite
        imgui::imgui
        spdlog::spdlog
        Catch2::Catch2WithMain
        concurrencpp::concurrencpp
)

target_include_directories(UnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_SOURCE_DIR}/test/utils
)
