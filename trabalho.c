#include <stdio.h>

typedef struct 
{
    int linha;
    int coluna;
    int qtd_movimentos;
    char tabuleiro[40][100];
}tMapa;

typedef struct 
{
    char comando;
}tJogada;

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

void ImprimeMapa(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            printf("%c", mapa.tabuleiro[i][j]);
        }
        printf("\n");
    }
}

int EhPacMan(char peca){
    return peca == '>';
}

int LinhaPacMan(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.tabuleiro[i][j])){
                return i+1;
            }
        }
    }
}

int ColunaPacMan(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.tabuleiro[i][j])){
                return j+1;
            }
        }
    }
}

tMapa InicializarJogo(){
    tMapa mapa;
    int lpacman, cpacman;

    mapa = LeMapa();

    lpacman = LinhaPacMan(mapa);
    cpacman = ColunaPacMan(mapa);
    //Gerar inicializacao.txt
    ImprimeMapa(mapa);
    printf("Pac-Man comecara o jogo na linha %d e coluna %d\n", lpacman, cpacman);
}

tJogada LeJogada(){
    tJogada jogada;

    scanf("%c\n", &jogada.comando);

    return jogada;
}

int EhComida(char peca){
    return peca == '*';
}

int RetornaQtdComida(tMapa mapa){
    int i, j, qtd_comida = 0;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhComida(mapa.tabuleiro[i][j])){
                qtd_comida++;
            }
        }
    }

    return qtd_comida;
}

tMapa MovimentaPacMan(tMapa mapa, tJogada jogada){

}

tMapa MovimentaFantasma(tMapa mapa){
    
}

void ImprimeEstadoAtual(tMapa mapa, tJogada jogada, int pontuacao){

}

void RealizarJogo(tMapa mapa){
    tJogada jogada;
    int i, pontuacao, qtd_comida_inicial, qtd_comida;

    qtd_comida_inicial = RetornaQtdComida(mapa);

    for(i=0; i<mapa.qtd_movimentos; i++){
        jogada = LeJogada();
        qtd_comida = RetornaQtdComida(mapa);
        mapa = MovimentaPacMan(mapa, jogada);
        mapa = MovimentaFantasma(mapa);
        if(RetornaQtdComida(mapa) < qtd_comida){
            pontuacao++;
        }
        ImprimeEstadoAtual(mapa, jogada, pontuacao);
    }
}

int main(){
    tMapa mapa;

    mapa = InicializarJogo();

    RealizarJogo(mapa);


    return 0;
}