cmake_minimum_required(VERSION 3.12)
project(Trabalho_De_IC)

# Configurações do executável
add_executable(main apps/main.cpp)
add_executable(main_automatic apps/main_automatic.cpp)

# Configurações de compilação (opcional)
set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# Adicione os arquivos .cpp da biblioteca MinhaBiblioteca ao executável
target_sources(main PRIVATE
    include/Node.h
    include/file_io.h
)

target_sources(main_automatic PRIVATE
    include/Node.h
    include/file_io.h
)

# Inclua o diretório que contém os cabeçalhos da biblioteca
target_include_directories(main PRIVATE include)
target_include_directories(main PRIVATE src)

target_include_directories(main_automatic PRIVATE include)
target_include_directories(main_automatic PRIVATE src)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION 1.0.0)
include(CPack)