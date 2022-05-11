
add_test(NAME "SingleInputFile"
         COMMAND reformat ${PROJECT_SOURCE_DIR}/test/basic.py single)
add_test(NAME "MultipleInputFiles"
         COMMAND reformat ${PROJECT_SOURCE_DIR}/test/basic.py multi)
add_test(NAME "MisalignedInputFiles"
         COMMAND reformat ${PROJECT_SOURCE_DIR}/test/basic.py misalign)

