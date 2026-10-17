if(NOT DEFINED INPUT OR NOT DEFINED OUTPUT)
    message(FATAL_ERROR "INPUT and OUTPUT are required")
endif()

file(READ "${INPUT}" PXLDOC_CONTENT)
file(WRITE "${OUTPUT}" "#pragma once\n\nnamespace pixelate_docs {\n\ninline constexpr const char* kEmbeddedPxldocs = R\"PXLDOC(")
file(APPEND "${OUTPUT}" "${PXLDOC_CONTENT}")
file(APPEND "${OUTPUT}" ")PXLDOC\";\n\n} // namespace pixelate_docs\n")
