# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/TheQuestofNoReturn_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/TheQuestofNoReturn_autogen.dir/ParseCache.txt"
  "TheQuestofNoReturn_autogen"
  )
endif()
