project(lab7_main CXX)
file(GLOB COMMON_SOURCES "ImageSet.cpp" "Interpolator.cpp" "CylindricalProjector.cpp" "OffsetEstimator.cpp" "ImageBlender.cpp" "Panorama.cpp")
add_project(lab7_main main.cpp ${COMMON_SOURCES})
# add_project(baseline Baseline.cpp ${COMMON_SOURCES})