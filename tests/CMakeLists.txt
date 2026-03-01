# Test configuration

set(PYTHON_EXECUTABLE python)

# Find bwc executable path
# Note: we assume bwc is in the same build tree
# On Windows/MSVC it might be in Release/bwc.exe or similar.

add_test(NAME CompilerValidation
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_runner.py --bin $<TARGET_FILE:bwc>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
