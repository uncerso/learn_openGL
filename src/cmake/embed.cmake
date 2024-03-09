add_executable(embed embed/source.cpp)

set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resources)
file(GLOB_RECURSE IMG_RESOURCES ${RESOURCES_DIR}/img/*)

set(IMG_RESOURCES_OUTPUT_DIR ${GENERATED_FILES}/resources/img)
make_directory(${IMG_RESOURCES_OUTPUT_DIR})

set(RESULT_IMG_RESOURCES)

foreach(IMG_RESOURCE ${IMG_RESOURCES})
    get_filename_component(RES_NAME ${IMG_RESOURCE} NAME_WLE)

    add_custom_command(
        OUTPUT ${RES_NAME}.h
        COMMAND embed ${IMG_RESOURCE} ${IMG_RESOURCES_OUTPUT_DIR}
        DEPENDS embed
    )

    set(RESULT_IMG_RESOURCES ${RESULT_IMG_RESOURCES} ${RES_NAME}.h)

endforeach()

add_custom_target(
    IMGResources ALL
    DEPENDS ${RESULT_IMG_RESOURCES}
)
