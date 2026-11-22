if(EXISTS "/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests")
  if(NOT EXISTS "/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests[1]_tests.cmake" OR
     NOT "/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests[1]_tests.cmake" IS_NEWER_THAN "/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests" OR
     NOT "/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests[1]_tests.cmake" IS_NEWER_THAN "${CMAKE_CURRENT_LIST_FILE}")
    include("/usr/share/cmake-4.2/Modules/GoogleTestAddTests.cmake")
    gtest_discover_tests_impl(
      TEST_EXECUTABLE [==[/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests]==]
      TEST_EXECUTOR [==[]==]
      TEST_WORKING_DIR [==[/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build]==]
      TEST_EXTRA_ARGS [==[]==]
      TEST_PROPERTIES [==[]==]
      TEST_PREFIX [==[]==]
      TEST_SUFFIX [==[]==]
      TEST_FILTER [==[]==]
      NO_PRETTY_TYPES [==[FALSE]==]
      NO_PRETTY_VALUES [==[FALSE]==]
      TEST_LIST [==[unit_tests_TESTS]==]
      CTEST_FILE [==[/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests[1]_tests.cmake]==]
      TEST_DISCOVERY_TIMEOUT [==[5]==]
      TEST_DISCOVERY_EXTRA_ARGS [==[]==]
      TEST_XML_OUTPUT_DIR [==[]==]
    )
  endif()
  include("/mnt/c/Users/shazi/Documents/cse 3205 Software Engineering/lab 6 project/CampusCare_SE_Pack/build/unit_tests[1]_tests.cmake")
else()
  add_test(unit_tests_NOT_BUILT unit_tests_NOT_BUILT)
endif()
