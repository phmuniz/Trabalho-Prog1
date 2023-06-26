#include <stdio.h>
#include "mapa.h"


tMapa LeMapa(){
    tMapa mapa;

    //ENTRADAS SERÃO POR ARQUIVO
    scanf("%d %d %d\n", &mapa.linha, &mapa.coluna, &mapa.qtd_movimentos);

    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            scanf("%c", &mapa.tabuleiro[i][j]);
        }
        scanf("%*c");
    }

    return mapa;
}