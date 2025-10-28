foreach (dir shaders assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${dir} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach ()
