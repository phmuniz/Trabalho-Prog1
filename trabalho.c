#include <stdio.h>

//Criando os tipos necessarios para o jogo
typedef struct 
{
    int linha;
    int coluna;
    int qtd_movimentos;
    char tabuleiro[40][100];
    char comida[40][100];
}tMapa;

typedef struct 
{
    int linha;
    int coluna;
    int pontos;
}tPacMan;

typedef struct 
{
    char tipo;
    int linha;
    int coluna;
    int direcao;
}tFantasma;

typedef struct 
{
    tMapa mapa;
    tPacMan pacman;
    tFantasma fantasmaB;
    tFantasma fantasmaP;
    tFantasma fantasmaI;
    tFantasma fantasmaC;
    char jogada;
    int bateuFantasma;
}tPartida;



//Funcao para ler as entradas dadas em mapa.txt
tMapa LeMapa(){
    tMapa mapa;

    //ENTRADAS SERÃO POR ARQUIVO
    scanf("%d %d %d\n", &mapa.linha, &mapa.coluna, &mapa.qtd_movimentos);

    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            scanf("%c", &mapa.tabuleiro[i][j]);

            if(mapa.tabuleiro[i][j] == '*'){
                mapa.comida[i][j] = '*';
            }else{
                mapa.comida[i][j] = ' ';
            }
        }
        scanf("\n");
    }

    return mapa;
}

//Funcao para imprimir o tabuleiro do mapa
void ImprimeMapa(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            printf("%c", mapa.tabuleiro[i][j]);
        }
        printf("\n");
    }
}

//Verifica se determinada peça é o PacMan
int EhPacMan(char peca){
    return peca == '>';
}

tPacMan InicializaPacMan(tMapa mapa){
    tPacMan pacman;

    pacman.pontos = 0;

    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.tabuleiro[i][j])){
                pacman.linha = i;
                pacman.coluna = j;
            }
        }
    }

    return pacman;
}

void GeraInicializacao(tPacMan pacman, tMapa mapa){
    ImprimeMapa(mapa);
    printf("Pac-Man comecara o jogo na linha %d e coluna %d\n", pacman.linha+1, 
    pacman.coluna+1);
}

tPartida InicializarJogo(){
    tPartida partida;

    partida.mapa = LeMapa();
    partida.pacman = InicializaPacMan(partida.mapa);
    partida.bateuFantasma = 0;

    //Gerar inicializacao.txt
    GeraInicializacao(partida.pacman, partida.mapa);

    return partida;
}

int EhComida(char peca){
    return peca == '*';
}

int RetornaQtdComida(tMapa mapa){
    int i, j, qtd_comida = 0;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhComida(mapa.comida[i][j])){
                qtd_comida++;
            }
        }
    }

    return qtd_comida;
}

tPartida MovimentaPacMan(tMapa mapa, tPacMan pacman, char jogada){
    tPartida partida;

    switch (jogada)
    {

    case 'w':
        if(mapa.tabuleiro[pacman.linha-1][pacman.coluna] == ' '){
            mapa.tabuleiro[pacman.linha-1][pacman.coluna] = '>';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.linha--;
        }
        else if(EhComida(mapa.tabuleiro[pacman.linha-1][pacman.coluna])){
            mapa.tabuleiro[pacman.linha-1][pacman.coluna] = '>';
            mapa.comida[pacman.linha-1][pacman.coluna] = ' ';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.linha--;
            pacman.pontos++;
        }
        break;

    case 's':
        if(mapa.tabuleiro[pacman.linha+1][pacman.coluna] == ' '){
            mapa.tabuleiro[pacman.linha+1][pacman.coluna] = '>';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.linha++;
        }
        else if(EhComida(mapa.tabuleiro[pacman.linha+1][pacman.coluna])){
            mapa.tabuleiro[pacman.linha+1][pacman.coluna] = '>';
            mapa.comida[pacman.linha+1][pacman.coluna] = ' ';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.linha++;
            pacman.pontos++;
        }
        break;

    case 'd':
        if(mapa.tabuleiro[pacman.linha][pacman.coluna+1] == ' '){
            mapa.tabuleiro[pacman.linha][pacman.coluna+1] = '>';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.coluna++;
        }
        else if(EhComida(mapa.tabuleiro[pacman.linha][pacman.coluna+1])){
            mapa.tabuleiro[pacman.linha][pacman.coluna+1] = '>';
            mapa.comida[pacman.linha][pacman.coluna+1] = ' ';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.coluna++;
            pacman.pontos++;
        }
        break;

    case 'a':
        if(mapa.tabuleiro[pacman.linha][pacman.coluna-1] == ' '){
            mapa.tabuleiro[pacman.linha][pacman.coluna-1] = '>';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.coluna--;
        }
        else if(EhComida(mapa.tabuleiro[pacman.linha][pacman.coluna-1])){
            mapa.tabuleiro[pacman.linha][pacman.coluna-1] = '>';
            mapa.comida[pacman.linha-1][pacman.coluna-1] = ' ';
            mapa.tabuleiro[pacman.linha][pacman.coluna] = ' ';
            pacman.coluna--;
            pacman.pontos++;
        }
        break;
    
    default:
        break;
    }

    partida.mapa = mapa;
    partida.pacman = pacman;
    partida.bateuFantasma = 0;
    partida.jogada = jogada;

    return partida;
}

tPartida MovimentaFantasma(tMapa mapa, tFantasma fantasma){
    
}

void ImprimeEstadoAtual(tMapa mapa, char jogada, int pontuacao){
    printf("Estado do jogo apos o movimento '%c':\n", jogada);
    ImprimeMapa(mapa);
    printf("Pontuacao: %d\n", pontuacao);
}

void ImprimeVitoria(int pontuacao){
    printf("Voce venceu!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void ImprimeDerrota(int pontuacao){
    printf("Game over!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void RealizarJogo(tPartida partida){
    int i, pontuacao = 0, qtd_comida_inicial;

    qtd_comida_inicial = RetornaQtdComida(partida.mapa);

    for(i=0; i<partida.mapa.qtd_movimentos; i++){
        scanf("%c\n", &partida.jogada);

        partida = MovimentaPacMan(partida.mapa, partida.pacman, partida.jogada);
        //partida = MovimentaFantasma(partida.mapa, partida.fantasmaB);
        
        ImprimeEstadoAtual(partida.mapa, partida.jogada, partida.pacman.pontos);

        if(qtd_comida_inicial == partida.pacman.pontos){
            ImprimeVitoria(partida.pacman.pontos);
            return;
        }
        if(partida.bateuFantasma){
            ImprimeDerrota(partida.pacman.pontos);
            return;
        }
    }

    if(qtd_comida_inicial == partida.pacman.pontos){
        ImprimeVitoria(partida.pacman.pontos);
    }else{
        ImprimeDerrota(partida.pacman.pontos);
    }
}

int main(){
    tPartida partida;

    partida = InicializarJogo();

    RealizarJogo(partida);


    return 0;
}