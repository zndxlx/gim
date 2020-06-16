cmake_minimum_required (VERSION 3.6)
project (sample)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Installation directory" FORCE)
set(SELF_LIBS_OUT ${CMAKE_SYSTEM_NAME}.out)

include_directories(.)
include_directories(WTL)

#include_directories(../..)
include_directories(../../mars)
include_directories(../../mars/comm/windows)
include_directories(../../mars/comm)
include_directories(../../mars/openssl/include)

include_directories(..)



#add_subdirectory(../../mars/log xlog.out)
#add_subdirectory(../../mars/app app.out)
#add_subdirectory(../../mars/baseevent baseevent.out)
#add_subdirectory(../../mars/boost boost.out)
#add_subdirectory(../../mars/comm comm.out)
#add_subdirectory(../../mars/sdt sdt.out)
#add_subdirectory(../../mars/stn stn.out)

add_definitions(-D_WIN32 -DWIN32 -DUNICODE -D_UNICODE)
add_definitions(-D_CRT_NO_VA_START_VALIDATION -D_CRT_SECURE_NO_WARNINGS)

set(CompilerFlags
	CMAKE_CXX_FLAGS
	CMAKE_CXX_FLAGS_DEBUG
	CMAKE_CXX_FLAGS_RELEASE
	CMAKE_C_FLAGS
	CMAKE_C_FLAGS_DEBUG
	CMAKE_C_FLAGS_RELEASE)
foreach(CompilerFlag ${CompilerFlags})
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set(SELF_SRC_FILES
		Business/ChatCGITask.cpp
		Business/MarsWrapper.cpp
		ChatDlg.cpp
		MainDlg.cpp
		PublicComponentV2/app_callback.cpp
		PublicComponentV2/longlink_packer.cc
		PublicComponentV2/shortlink_packer.cc
		PublicComponentV2/stn_callback.cpp
		Sample.cpp
		stdafx.cpp
		Wrapper/NetworkService.cpp)
    

add_executable(${PROJECT_NAME} WIN32 ${SELF_SRC_FILES} Sample.rc)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${SELF_LIBS_OUT})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
                
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/GL /Zc:wchar_t")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LTCG")

  
if(CMAKE_CL_64)
	add_definitions(-D_WIN64 -DWIN64)
endif()

set(CMAKE_FIND_LIBRARY_PREFIXES "")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

link_directories(${SELF_LIBS_OUT})

target_link_libraries(${PROJECT_NAME} ws2_32.lib comsuppw.lib
						../mars/xlog
                        ../mars/app
                        ../mars/baseevent
                        ../mars/mars-boost
                        ../mars/comm
                        ../mars/sdt
                        ../mars/stn)
  