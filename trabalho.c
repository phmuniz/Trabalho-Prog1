#include <stdio.h>

//Criando os tipos necessarios para o jogo
typedef struct 
{
    int linha;
    int coluna;
    int qtd_movimentos;
    char tabuleiro[40][100];
    char comida[40][100];
    char pacman[40][100];
    char fantasma[40][100];
}tMapa;

typedef struct 
{
    char comando;
}tJogada;

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

            if(mapa.tabuleiro[i][j] == '#'){
                mapa.pacman[i][j] = '#';
                mapa.fantasma[i][j] = '#';
            }else if(mapa.tabuleiro[i][j] == '>'){
                mapa.pacman[i][j] = '>';
            }else if(mapa.tabuleiro[i][j] == 'B'){
                mapa.fantasma[i][j] = 'B';
            }else if(mapa.tabuleiro[i][j] == 'P'){
                mapa.fantasma[i][j] = 'P';
            }else if(mapa.tabuleiro[i][j] == 'I'){
                mapa.fantasma[i][j] = 'I';
            }else if(mapa.tabuleiro[i][j] == 'C'){
                mapa.fantasma[i][j] = 'C';
            }else{
                mapa.pacman[i][j] = ' ';
                mapa.fantasma[i][j] = ' ';
            }
        }
        scanf("%*c");
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

//Retorna a linha do tabuleiro em que esta o PacMan
int LinhaPacMan(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.pacman[i][j])){
                return i;
            }
        }
    }
}

//Retorna a coluna do tabuleiro em que esta o PacMan
int ColunaPacMan(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.pacman[i][j])){
                return j;
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
    printf("Pac-Man comecara o jogo na linha %d e coluna %d\n", lpacman+1, cpacman+1);
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
            if(EhComida(mapa.comida[i][j])){
                qtd_comida++;
            }
        }
    }

    return qtd_comida;
}

tMapa MovimentaPacMan(tMapa mapa, tJogada jogada){
    int lpacman, cpacman;

    lpacman = LinhaPacMan(mapa);
    cpacman = ColunaPacMan(mapa);

    switch (jogada.comando)
    {

    case 'w':
        if(mapa.pacman[lpacman-1][cpacman] != '#'){
            mapa.pacman[lpacman-1][cpacman] = '>';
            mapa.pacman[lpacman][cpacman] = ' ';
        }
        break;

    case 's':
        if(mapa.pacman[lpacman+1][cpacman] != '#'){
            mapa.pacman[lpacman+1][cpacman] = '>';
            mapa.pacman[lpacman][cpacman] = ' ';
        }
        break;

    case 'd':
        if(mapa.pacman[lpacman][cpacman+1] != '#'){
            mapa.pacman[lpacman][cpacman+1] = '>';
            mapa.pacman[lpacman][cpacman] = ' ';
        }
        break;

    case 'a':
        if(mapa.pacman[lpacman][cpacman-1] != '#'){
            mapa.pacman[lpacman][cpacman-1] = '>';
            mapa.pacman[lpacman][cpacman] = ' ';
        }
        break;
    
    default:
        break;
    }
}

tMapa MovimentaFantasma(tMapa mapa){
    
}

tMapa AtualizaMovimentos(tMapa mapa){

}

void ImprimeEstadoAtual(tMapa mapa, tJogada jogada, int pontuacao){
    printf("Estado do jogo apos o movimento '%c':\n", jogada.comando);
    ImprimeMapa(mapa);
    printf("Pontuacao: %d\n", pontuacao);
}

int PacManEstaNoMapa(tMapa mapa){
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(EhPacMan(mapa.tabuleiro[i][j])){
                return 1;
            }
        }
    }

    return 0;
}

int Ganhou(tMapa mapa, int qtd_comida_inicial, int pontuacao){
    if(PacManEstaNoMapa(mapa) && qtd_comida_inicial == pontuacao){
        return 1;
    }
    return 0;
}

int Perdeu(tMapa mapa, int qtd_comida_inicial, int pontuacao){
    if(PacManEstaNoMapa(mapa) == 0 && qtd_comida_inicial != pontuacao){
        return 1;
    }
    return 0;
}

void ImprimeVitoria(int pontuacao){
    printf("Voce venceu!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void ImprimeDerrota(int pontuacao){
    printf("Game over!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void RealizarJogo(tMapa mapa){
    tJogada jogada;
    int i, pontuacao = 0, qtd_comida_inicial, qtd_comida;

    qtd_comida_inicial = RetornaQtdComida(mapa);

    for(i=0; i<mapa.qtd_movimentos; i++){

        jogada = LeJogada();

        qtd_comida = RetornaQtdComida(mapa);

        mapa = MovimentaPacMan(mapa, jogada);
        mapa = MovimentaFantasma(mapa);
        mapa = AtualizaMovimentos(mapa);

        if(RetornaQtdComida(mapa) < qtd_comida){
            pontuacao++;
        }
        
        ImprimeEstadoAtual(mapa, jogada, pontuacao);

        if(Ganhou(mapa, qtd_comida_inicial, pontuacao)){
            ImprimeVitoria(pontuacao);
            return;
        }
        if(Perdeu(mapa, qtd_comida_inicial, pontuacao)){
            ImprimeDerrota(pontuacao);
            return;
        }
    }

    if(Ganhou(mapa, qtd_comida_inicial, pontuacao)){
        ImprimeVitoria(pontuacao);
    }
    else if(Perdeu(mapa, qtd_comida_inicial, pontuacao)){
        ImprimeDerrota(pontuacao);
    }
}

int main(){
    tMapa mapa;

    mapa = InicializarJogo();

    RealizarJogo(mapa);


    return 0;
}