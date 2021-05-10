find_package(OpenCL REQUIRED)

include_directories(${TOP_DIR}/ThirdParty/include ${OpenCL_INCLUDE_DIRS})

if(NOT Build_C_Wrapper_With_Core)
    target_link_libraries(${PROJECT_NAME} Anime4KCPPCore)
else()
    include(${TOP_DIR}/cmake/ThirdPartyForCore.cmake)
endif()
