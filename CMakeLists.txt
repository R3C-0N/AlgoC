cmake_minimum_required(VERSION 3.7)
project(AlgoC)

set(CMAKE_CXX_STANDARD 14)

include_directories(Projet/src)

add_executable(AlgoC
        Projet/src/serveur.c
        Projet/src/serveur.h
        Projet/src/client.c
        Projet/src/client.h
        Projet/src/bmp.c
        Projet/src/bmp.h
        Projet/src/couleur.c
        Projet/src/couleur.h
        Projet/src/validateur.c
        Projet/src/validateur.h
        )
