# gonna use a glob to collect header files
file(GLOB_RECURSE HEADER_FILES "*.hpp")

# the source_group is for CMakeLists "virtual folders" so it's not necessary but helps
source_group("Header Files" FILES ${HEADER_FILES})

set(HEADER_FILES ${HEADER_FILES} PARENT_SCOPE)