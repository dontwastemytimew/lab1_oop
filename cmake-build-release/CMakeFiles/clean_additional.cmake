# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\CoreLib_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CoreLib_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\QCustomPlotLib_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\QCustomPlotLib_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\lab3_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\lab3_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\lab3_benchmarks_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\lab3_benchmarks_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\lab3_tests_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\lab3_tests_autogen.dir\\ParseCache.txt"
  "CoreLib_autogen"
  "QCustomPlotLib_autogen"
  "lab3_autogen"
  "lab3_benchmarks_autogen"
  "lab3_tests_autogen"
  )
endif()
