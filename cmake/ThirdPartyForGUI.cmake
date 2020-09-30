find_package(OpenCV REQUIRED)
find_package(OpenCL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE Anime4KCPPCore)
