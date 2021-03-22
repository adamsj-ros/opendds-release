cmake_minimum_required(VERSION 3.5)

project(opendds)

execute_process(
  COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --doc-group
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/opendds"
  OUTPUT_FILE "configure.log"
  ERROR_FILE "configure_err.log"
  )

  execute_process(
  COMMAND make -j
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/opendds"
  OUTPUT_FILE "make.log"
  ERROR_FILE "make_err.log"
)

execute_process(
  COMMAND make install
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/opendds"
  OUTPUT_FILE "make_install.log"
  ERROR_FILE "make_install_err.log"
)

