cmake_minimum_required(VERSION 3.13.0)

include(CMakeParseArguments)

function(generate_launcher_version)
  set(options FAST)
  set(oneValueArgs
      VERSION_FILE
      HEADER_TEMPLATE
      HEADER_OUT
      GIT_DIR)
  set(multiValueArgs)
  cmake_parse_arguments(GEN
                        "${options}"
                        "${oneValueArgs}"
                        "${multiValueArgs}"
                        ${ARGN})

  set(APP_VERSION "0.0.0")
  set(GIT_BRANCH "unknown")
  set(GIT_COMMIT "unknown")

  if(EXISTS ${GEN_VERSION_FILE})
    file(READ ${GEN_VERSION_FILE} APP_VERSION)
    string(STRIP ${APP_VERSION} APP_VERSION)
    message(STATUS "APP_VERSION: " ${APP_VERSION})
  else()
    message(
      WARNING "'${GEN_VERSION_FILE}' does not exist, unable to get app version."
      )
  endif()

  find_package(Git)

  if(GIT_FOUND AND EXISTS ${GEN_GIT_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE}
                            rev-parse
                            --short=8
                            HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE exit_code
                    OUTPUT_VARIABLE GIT_COMMIT_HASH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${exit_code} EQUAL 0)
      message(WARNING "git rev-parse failed, unable to include commit hash.")
    endif()

    execute_process(COMMAND ${GIT_EXECUTABLE}
                            rev-parse
                            --abbrev-ref
                            HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE exit_code
                    OUTPUT_VARIABLE GIT_BRANCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${exit_code} EQUAL 0)
      message(WARNING "git rev-parse failed, unable to include git branch.")
    endif()

    message(STATUS "GIT_COMMIT_HASH: " ${GIT_COMMIT_HASH})
    message(STATUS "GIT_BRANCH: " ${GIT_BRANCH})
  else()
    message(WARNING "git not found, unable to include version.")
  endif()

  set(APP_VERSION ${APP_VERSION} PARENT_SCOPE)
  set(GIT_BRANCH ${GIT_BRANCH} PARENT_SCOPE)
  set(GIT_COMMIT ${GIT_COMMIT} PARENT_SCOPE)

  configure_file(${GEN_HEADER_TEMPLATE} ${GEN_HEADER_OUT})

endfunction(generate_launcher_version)
