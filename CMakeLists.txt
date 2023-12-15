cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME SDSO)
project(${PROJECT_NAME})
#set(CMAKE_VERBOSE_MAKEFILE ON)


#set(BUILD_TYPE Release)
set(BUILD_TYPE RelWithDebInfo)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# required libraries
find_package(SuiteSparse REQUIRED)
find_package(Eigen3 REQUIRED)

# optional libraries
find_package(LibZip QUIET)
find_package(Pangolin QUIET)
find_package(OpenCV QUIET)

# Sources files
set(dso_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/FullSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/FullSystemOptimize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/FullSystemOptPoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/FullSystemDebugStuff.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/FullSystemMarginalize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/Residuals.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/CoarseTracker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/CoarseInitializer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/ImmaturePoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/HessianBlocks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/FullSystem/PixelSelector2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/OptimizationBackend/EnergyFunctional.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/OptimizationBackend/AccumulatedTopHessian.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/OptimizationBackend/AccumulatedSCHessian.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/OptimizationBackend/EnergyFunctionalStructs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util/settings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util/Undistort.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util/globalCalib.cpp
)

# compile main library.
add_library(dso ${dso_SOURCE_FILES})
target_compile_features(dso PRIVATE cxx_std_17)
target_compile_definitions(dso PUBLIC ENABLE_SSE)
target_compile_options(dso PRIVATE -O3 -fPIC -march=native)
target_include_directories(dso
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Sophus
	PRIVATE ${CXPARSE_INCLUDE_DIR}
	PRIVATE ${CHOLMOD_INCLUDE_DIR})
target_link_libraries(dso
	PUBLIC Eigen3::Eigen)

# decide if we have pangolin
if (Pangolin_FOUND)
	message("--- found PANGOLIN, compiling dso_pangolin library.")
	set(dso_pangolin_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/Pangolin/KeyFrameDisplay.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/Pangolin/PangolinDSOViewer.cpp)
	target_sources(dso PRIVATE ${dso_pangolin_SOURCE_FILES})
	target_include_directories(dso PUBLIC ${Pangolin_INCLUDE_DIRS})
	target_link_directories(dso PRIVATE ${Pangolin_LIBRARY_DIRS})
	target_link_libraries(dso
		PRIVATE pango_display
		PRIVATE pango_opengl)
	set(HAS_PANGOLIN 1)
else ()
	message("--- could not find PANGOLIN, not compiling dso_pangolin library.")
	message("    this means there will be no 3D display / GUI available for dso_dataset.")
	set(dso_pangolin_SOURCE_FILES "")
	set(HAS_PANGOLIN 0)
endif ()

# decide if we have openCV
if (OpenCV_FOUND)
	message("--- found OpenCV, compiling dso_opencv library.")
	include_directories( ${OpenCV_INCLUDE_DIRS} )
	set(dso_opencv_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/OpenCV/ImageDisplay_OpenCV.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/OpenCV/ImageRW_OpenCV.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/OpenCV/DepthImageWrapper.cpp)
  target_sources(dso PRIVATE ${dso_opencv_SOURCE_FILES})
  target_include_directories(dso PUBLIC ${OpenCV_INCLUDE_DIRS})
  target_link_directories(dso PRIVATE ${OpenCV_LINK_DIRS})
  target_link_libraries(dso PRIVATE ${OpenCV_LIBS})
	set(HAS_OPENCV 1)
else ()
	message("--- could not find OpenCV, not compiling dso_opencv library.")
	message("    this means there will be no image display, and image read / load functionality.")
	set(dso_opencv_SOURCE_FILES
	  ${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/ImageDisplay_dummy.cpp
	  ${CMAKE_CURRENT_SOURCE_DIR}/src/IOWrapper/ImageRW_dummy.cpp)
	target_sources(dso PRIVATE ${dso_opencv_SOURCE_FILES})
	set(HAS_OPENCV 0)
endif ()

# decide if we have ziplib.
if (LIBZIP_LIBRARY)
	message("--- found ziplib (${LIBZIP_VERSION}), compiling with zip capability.")
	target_compile_definitions(dso PRIVATE HAS_ZIPLIB=1)
	target_include_directories(dso PRIVATE ${LIBZIP_INCLUDE_DIR_ZIP} ${LIBZIP_INCLUDE_DIR_ZIPCONF})
else()
	message("--- not found ziplib (${LIBZIP_LIBRARY}), compiling without zip capability.")
	set(LIBZIP_LIBRARY "")
endif()

# build main executable (only if we have both OpenCV and Pangolin)
if (OpenCV_FOUND AND Pangolin_FOUND)
	message("--- compiling dso_dataset.")
	add_executable(dso_dataset ${CMAKE_CURRENT_SOURCE_DIR}/src/main_dso_pangolin.cpp )
	target_compile_options(dso_dataset PRIVATE -O3 -fPIE -pie -march=native)
	target_link_libraries(dso_dataset
		dso
		cxsparse
		${LIBZIP_LIBRARY}
		pango_display
		pango_opengl
		${OpenCV_LIBS})
else()
	message("--- not building dso_dataset, since either don't have openCV or Pangolin.")
endif()
