cmake_minimum_required(VERSION 4.4.3)

project("lab-1" CXX)

add_executable("${PROJECT_NAME}")
target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_23)
target_sources("${PROJECT_NAME}"
    PRIVATE
      "main.cpp"
    PRIVATE FILE_SET "modules" TYPE CXX_MODULES FILES
      "matrix.cppm"
      "random.cppm"
)