set(ACCEL_SOURCES ${ACCEL_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/accel.cc
    ${CMAKE_CURRENT_LIST_DIR}/kd_tree_accel.cc
    ${CMAKE_CURRENT_LIST_DIR}/bbvh_accel.cc
    ${CMAKE_CURRENT_LIST_DIR}/qbvh_accel.cc)

set(ACCEL_HEADERS ${ACCEL_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/accel.h
    ${CMAKE_CURRENT_LIST_DIR}/accel_interface.h
    ${CMAKE_CURRENT_LIST_DIR}/kd_tree_accel.h
    ${CMAKE_CURRENT_LIST_DIR}/bbvh_accel.h
    ${CMAKE_CURRENT_LIST_DIR}/qbvh_accel.h)

set(SOURCES ${SOURCES} ${ACCEL_SOURCES} PARENT_SCOPE)
set(HEADERS ${HEADERS} ${ACCEL_HEADERS} PARENT_SCOPE)
