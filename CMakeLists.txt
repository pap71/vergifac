cmake_minimum_required(VERSION 3.10)
project(vergifac C)

# Répertoires des sources et includes
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/user-vergifac)

# Fichiers sources
set(SOURCES
    ${SRC_DIR}/fastmenu.c
    ${SRC_DIR}/saiscli.c
    ${SRC_DIR}/mesfast.c
    ${SRC_DIR}/sqlcliDB.c
    ${SRC_DIR}/listcli.c
    ${SRC_DIR}/listprod.c
    ${SRC_DIR}/saiprod.c
    ${SRC_DIR}/paramsfac.c
    ${SRC_DIR}/memodoc.c
    ${SRC_DIR}/editfast.c
    ${SRC_DIR}/saifaccod.c
    ${SRC_DIR}/saifac.c
    ${UTIL_DIR}/genftablo.c
    ${UTIL_DIR}/genfq.c
    ${UTIL_DIR}/genctrl.c
    ${UTIL_DIR}/gencbox.c
)

# Fichiers d'en-tête (optionnel, pour la visibilité)
set(HEADERS
    ${SRC_DIR}/fast.h
)

# Créer l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Inclure les répertoires util/ et GTK+
target_include_directories(${PROJECT_NAME} PRIVATE
    ${UTIL_DIR}
    ${GTK2_INCLUDE_DIRS}
)

# Flags de compilation
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-deprecated-declarations )

# Trouver GTK+ et SQLite
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
# Après pkg_check_modules
include_directories(${GTK2_INCLUDE_DIRS})

# Lier les bibliothèques
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GTK2_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    m
)

# Ancienne version (incorrecte si user-vergifac est à la racine)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/user-vergifac)

# Nouvelle version (corrigée)  Définir le répertoire de sortie
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/user-vergifac)

