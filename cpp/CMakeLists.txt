set(main-lib cpp2android-lib)
target_include_directories(${main-lib} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bootstrap)

# Add source files here
# Use ${main-lib}
target_sources(${main-lib} PRIVATE
     ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)