# ENet configuration
set(ENET_STATIC ON CACHE BOOL "" FORCE)
set(ENET_SHARED OFF CACHE BOOL "" FORCE)
set(ENET_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/enet)

include_directories(extern/ska_sort EXCLUDE_FROM_ALL)

find_package(SDL2 REQUIRED)
find_package(fastnoise-lite REQUIRED)
find_package(spdlog REQUIRED)
find_package(concurrencpp REQUIRED)
find_package(cpptrace REQUIRED)
find_package(tsl-hopscotch-map REQUIRED)
find_package(MagnumExtras REQUIRED Ui)