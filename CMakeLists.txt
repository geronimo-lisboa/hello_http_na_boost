cmake_minimum_required(VERSION 2.8)

project(TesteHttpServer)

SET (BOOST_LIBRARY_DIR "C:/boost_1_64_0/stage/lib")
SET (BOOST_INCLUDE_DIR "C:/boost_1_64_0")

file(GLOB my_source_files "*.h" "*.cpp")

add_executable(teste_http_server ${my_source_files})
include_directories(${BOOST_INCLUDE_DIR})

target_link_libraries(teste_http_server "C:/boost_1_64_0/stage/lib/libboost_system-vc120-mt-gd-1_64.lib")
target_link_libraries(teste_http_server "C:/boost_1_64_0/stage/lib/libboost_date_time-vc120-mt-gd-1_64.lib")
target_link_libraries(teste_http_server "C:/boost_1_64_0/stage/lib/libboost_regex-vc120-mt-gd-1_64.lib")
#add_library (vr ${my_source_files} ) 
# Expose B's public includes (including Boost transitively) to other
# subprojects through cache variable.
#set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
#    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
#include_directories(${BASE_INCLUDE_DIRS})
#target_link_libraries(vr base)
#include_directories(${SolidoDeExtrusao_INCLUDE_DIRS})
#target_link_libraries(vr solidoDeExtrusao)
