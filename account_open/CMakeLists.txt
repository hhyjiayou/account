set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/account-open/lib)

########################include dispatcher########################
include_directories(src)
########################source c2s########################
aux_source_directory(src MANAGER_SRC)
########################generate account-c2s.lib####################
add_library(knights-account-open SHARED ${MANAGER_SRC})