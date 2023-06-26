#ifndef MAPA_H
#define MAPA_H

#include <stdio.h>

typedef struct 
{
    int linha;
    int coluna;
    int qtd_movimentos;
    char tabuleiro[40][100];
}tMapa;

tMapa LeMapa();

#endif