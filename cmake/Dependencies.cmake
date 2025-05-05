include_directories(extern/ska_sort EXCLUDE_FROM_ALL)

find_package(SDL2 REQUIRED)
find_package(fastnoise-lite REQUIRED)
find_package(spdlog REQUIRED)
find_package(concurrencpp REQUIRED)
find_package(cpptrace REQUIRED)
find_package(tsl-hopscotch-map REQUIRED)
find_package(MagnumExtras REQUIRED Ui)