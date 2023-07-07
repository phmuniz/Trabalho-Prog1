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
    int existe; // O se não existe, 1 se existe
    int linha;
    int coluna;
    int direcao; // 0 direção inicial, 1 direção contrária 
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
    int bateuFantasma; // 0 se não bateu, 1 se bateu
    int colidiuParede; // 0 se não colidiu, 1 se colidiu 
    int pegouComida; // 0 se não pegou, 1 se pegou
    int qtdMovimentoW;
    int qtdMovimentoS;
    int qtdMovimentoD;
    int qtdMovimentoA;
    int trilha[40][100];
}tPartida;

//Funcao para ler as entradas dadas em mapa.txt
tMapa LeMapa(char * argv[]){
    tMapa mapa;
    char caminho[1000];

    sprintf(caminho,"%s/mapa.txt",argv[1]);

    FILE * entrada = fopen(caminho,"r");

    if (entrada == NULL){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado");
    }

    fscanf(entrada, "%d %d %d\n", &mapa.linha, &mapa.coluna, &mapa.qtd_movimentos);

    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            fscanf(entrada, "%c", &mapa.tabuleiro[i][j]);

            if(mapa.tabuleiro[i][j] == '*'){
                mapa.comida[i][j] = '*';
            }else{
                mapa.comida[i][j] = ' ';
            }
        }
        fscanf(entrada, "%*c");
    }

    fclose(entrada);

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

int EhFantasma(char peca){
    return peca == 'B' || peca == 'P' || peca == 'I' || peca == 'C';
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

tFantasma InicializaFantasma(tMapa mapa, char tipo){
    tFantasma fantasma;

    fantasma.existe = 0;
    fantasma.direcao = 0;

    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            if(mapa.tabuleiro[i][j] == tipo){
                fantasma.linha = i;
                fantasma.coluna = j;
                fantasma.existe = 1;

                return fantasma;
            }
        }
    }

    return fantasma;
}

void GeraInicializacao(tPacMan pacman, tMapa mapa, char * argv[]){
    char caminho[1000];

    sprintf(caminho,"%s/saida/inicializacao.txt",argv[1]);

    FILE * inicializacao = fopen(caminho,"w");
    
    int i, j;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            fprintf(inicializacao, "%c", mapa.tabuleiro[i][j]);
        }
        fprintf(inicializacao, "\n");
    }

    fprintf(inicializacao, "Pac-Man comecara o jogo na linha %d e coluna %d\n", pacman.linha+1, 
    pacman.coluna+1);

    fclose(inicializacao);
}

tPartida InicializarJogo(char * argv[]){
    tPartida partida;

    partida.mapa = LeMapa(argv);
    partida.pacman = InicializaPacMan(partida.mapa);
    partida.fantasmaB = InicializaFantasma(partida.mapa, 'B');
    partida.fantasmaP = InicializaFantasma(partida.mapa, 'P');
    partida.fantasmaI = InicializaFantasma(partida.mapa, 'I');
    partida.fantasmaC = InicializaFantasma(partida.mapa, 'C');
    partida.bateuFantasma = 0;
    partida.colidiuParede = 0;
    partida.pegouComida = 0;
    partida.qtdMovimentoW = 0;
    partida.qtdMovimentoS = 0;
    partida.qtdMovimentoD = 0;
    partida.qtdMovimentoA = 0;
    
    //Inicializa Trilha
    int i, j;
    for(i=0; i<partida.mapa.linha; i++){
        for(j=0; j<partida.mapa.coluna; j++){
            partida.trilha[i][j] = -1;
            if(partida.pacman.linha == i && partida.pacman.coluna == j){
                partida.trilha[i][j] = 0;
            }
        }
    }

    //Gerar inicializacao.txt
    GeraInicializacao(partida.pacman, partida.mapa, argv);

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

tPartida MovimentaFantasma(tPartida partida){

    //Verifica se o Fantasma B existe, se existe movimenta ele
    if(partida.fantasmaB.existe){

        if(partida.fantasmaB.direcao == 0){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1]) || 
            partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1]) && partida.jogada == 'd'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoD++;
                }
                partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1] = 'B';
                if(EhComida(partida.mapa.comida[partida.fantasmaB.linha][partida.fantasmaB.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = ' ';
                }
                partida.fantasmaB.coluna--;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1] = 'B';
                if(EhComida(partida.mapa.comida[partida.fantasmaB.linha][partida.fantasmaB.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = ' ';
                }
                partida.fantasmaB.coluna++;
                partida.fantasmaB.direcao = 1;
            }
        }

        else if(partida.fantasmaB.direcao){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1]) || 
            partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1]) && partida.jogada == 'a'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoA++;
                }
                partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1] = 'B';
                if(EhComida(partida.mapa.comida[partida.fantasmaB.linha][partida.fantasmaB.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = ' ';
                }
                partida.fantasmaB.coluna++;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna+1] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna-1] = 'B';
                if(EhComida(partida.mapa.comida[partida.fantasmaB.linha][partida.fantasmaB.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaB.linha][partida.fantasmaB.coluna] = ' ';
                }
                partida.fantasmaB.coluna--;
                partida.fantasmaB.direcao = 0;
            }
        }
    }

    //Verifica se o Fantasma P existe, se existe movimenta ele
    if(partida.fantasmaP.existe){

        if(partida.fantasmaP.direcao == 0){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna]) || 
            partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna]) && partida.jogada == 's'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoS++;
                }
                partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna] = 'P';
                if(EhComida(partida.mapa.comida[partida.fantasmaP.linha][partida.fantasmaP.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = ' ';
                }
                partida.fantasmaP.linha--;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna] = 'P';
                if(EhComida(partida.mapa.comida[partida.fantasmaP.linha][partida.fantasmaP.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = ' ';
                }
                partida.fantasmaP.linha++;
                partida.fantasmaP.direcao = 1;
            }
        }

        else if(partida.fantasmaP.direcao){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna]) || 
            partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna]) && partida.jogada == 'w'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoW++;
                }
                partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna] = 'P';
                if(EhComida(partida.mapa.comida[partida.fantasmaP.linha][partida.fantasmaP.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = ' ';
                }
                partida.fantasmaP.linha++;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaP.linha+1][partida.fantasmaP.coluna] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaP.linha-1][partida.fantasmaP.coluna] = 'P';
                if(EhComida(partida.mapa.comida[partida.fantasmaP.linha][partida.fantasmaP.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaP.linha][partida.fantasmaP.coluna] = ' ';
                }
                partida.fantasmaP.linha--;
                partida.fantasmaP.direcao = 0;
            }
        }
    }

    //Verifica se o Fantasma I existe, se existe movimenta ele
    if(partida.fantasmaI.existe){

        if(partida.fantasmaI.direcao == 0){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna]) || 
            partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna]) && partida.jogada == 'w'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoW++;
                }
                partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna] = 'I';
                if(EhComida(partida.mapa.comida[partida.fantasmaI.linha][partida.fantasmaI.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = ' ';
                }
                partida.fantasmaI.linha++;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna] = 'I';
                if(EhComida(partida.mapa.comida[partida.fantasmaI.linha][partida.fantasmaI.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = ' ';
                }
                partida.fantasmaI.linha--;
                partida.fantasmaI.direcao = 1;
            }
        }

        else if(partida.fantasmaI.direcao){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna]) || 
            partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna]) && partida.jogada == 's'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoS++;
                }
                partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna] = 'I';
                if(EhComida(partida.mapa.comida[partida.fantasmaI.linha][partida.fantasmaI.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = ' ';
                }
                partida.fantasmaI.linha--;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaI.linha-1][partida.fantasmaI.coluna] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaI.linha+1][partida.fantasmaI.coluna] = 'I';
                if(EhComida(partida.mapa.comida[partida.fantasmaI.linha][partida.fantasmaI.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaI.linha][partida.fantasmaI.coluna] = ' ';
                }
                partida.fantasmaI.linha++;
                partida.fantasmaI.direcao = 0;
            }
        }
    }

    //Verifica se o Fantasma C existe, se existe movimenta ele
    if(partida.fantasmaC.existe){

        if(partida.fantasmaC.direcao == 0){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1]) || 
            partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1]) && partida.jogada == 'a'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoA++;
                }
                partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1] = 'C';
                if(EhComida(partida.mapa.comida[partida.fantasmaC.linha][partida.fantasmaC.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = ' ';
                }
                partida.fantasmaC.coluna++;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1] = 'C';
                if(EhComida(partida.mapa.comida[partida.fantasmaC.linha][partida.fantasmaC.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = ' ';
                }
                partida.fantasmaC.coluna--;
                partida.fantasmaC.direcao = 1;
            }
        }

        else if(partida.fantasmaC.direcao){
            if(EhComida(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1]) || 
            EhPacMan(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1]) || 
            partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1] == ' '){
                if(EhPacMan(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1]) && partida.jogada == 'd'){
                    partida.bateuFantasma = 1;
                    partida.qtdMovimentoD++;
                }
                partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1] = 'C';
                if(EhComida(partida.mapa.comida[partida.fantasmaC.linha][partida.fantasmaC.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = ' ';
                }
                partida.fantasmaC.coluna--;
            }
            else if(partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna-1] == '#'){
                partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna+1] = 'C';
                if(EhComida(partida.mapa.comida[partida.fantasmaC.linha][partida.fantasmaC.coluna])){
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = '*';
                }else{
                    partida.mapa.tabuleiro[partida.fantasmaC.linha][partida.fantasmaC.coluna] = ' ';
                }
                partida.fantasmaC.coluna++;
                partida.fantasmaC.direcao = 0;
            }
        }
    }

    return partida;
}

tPartida MovimentaPacMan(tPartida partida){

    switch (partida.jogada)
    {

    case 'w':
        partida.qtdMovimentoW++;

        if(partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna] == ' '){
            partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna] = '>';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.linha--;
        }
        else if(EhComida(partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna])){
            partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna] = '>';
            partida.mapa.comida[partida.pacman.linha-1][partida.pacman.coluna] = ' ';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.linha--;
            partida.pacman.pontos++;
            partida.pegouComida = 1;
        }
        else if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna])){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.bateuFantasma = 1;
        }
        else if(partida.mapa.tabuleiro[partida.pacman.linha-1][partida.pacman.coluna] == '#'){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna])){
                partida.bateuFantasma = 1;
            }
            partida.colidiuParede = 1;
        }
        break;

    case 's':
        partida.qtdMovimentoS++;

        if(partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna] == ' '){
            partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna] = '>';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.linha++;
        }
        else if(EhComida(partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna])){
            partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna] = '>';
            partida.mapa.comida[partida.pacman.linha+1][partida.pacman.coluna] = ' ';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.linha++;
            partida.pacman.pontos++;
            partida.pegouComida = 1;
        }
        else if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna])){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.bateuFantasma = 1;
        }
        else if(partida.mapa.tabuleiro[partida.pacman.linha+1][partida.pacman.coluna] == '#'){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna])){
                partida.bateuFantasma = 1;
            }
            partida.colidiuParede = 1;
        }
        break;

    case 'd':
        partida.qtdMovimentoD++;

        if(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1] == ' '){
            partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1] = '>';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.coluna++;
        }
        else if(EhComida(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1])){
            partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1] = '>';
            partida.mapa.comida[partida.pacman.linha][partida.pacman.coluna+1] = ' ';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.coluna++;
            partida.pacman.pontos++;
            partida.pegouComida = 1;
        }
        else if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1])){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.bateuFantasma = 1;
        }
        else if(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna+1] == '#'){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna])){
                partida.bateuFantasma = 1;
            }
            partida.colidiuParede = 1;
        }
        break;

    case 'a':
        partida.qtdMovimentoA++;

        if(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1] == ' '){
            partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1] = '>';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.coluna--;
        }
        else if(EhComida(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1])){
            partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1] = '>';
            partida.mapa.comida[partida.pacman.linha][partida.pacman.coluna-1] = ' ';
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.pacman.coluna--;
            partida.pacman.pontos++;
            partida.pegouComida = 1;
        }
        else if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1])){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna]) == 0){
                partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna] = ' ';
            }
            partida.bateuFantasma = 1;
        }
        else if(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna-1] == '#'){
            if(EhFantasma(partida.mapa.tabuleiro[partida.pacman.linha][partida.pacman.coluna])){
                partida.bateuFantasma = 1;
            }
            partida.colidiuParede = 1;
        }
        break;
    
    default:
        break;
    }

    return partida;
}

void ImprimeEstadoAtual(tMapa mapa, char jogada, int pontuacao){
    printf("Estado do jogo apos o movimento '%c':\n", jogada);
    ImprimeMapa(mapa);
    printf("Pontuacao: %d\n", pontuacao);
    printf("\n");
}

void ImprimeVitoria(int pontuacao){
    printf("Voce venceu!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void ImprimeDerrota(int pontuacao){
    printf("Game over!\n");
    printf("Pontuacao final: %d\n", pontuacao);
}

void GeraEstatisticas(char * argv[], tPartida partida, int qtd_colisoes_parede, int qtd_movimentos){
    char caminho[1000];

    sprintf(caminho,"%s/saida/estatisticas.txt",argv[1]);

    FILE * estatisticas = fopen(caminho,"w");

    fprintf(estatisticas, "Numero de movimentos: %d\n", qtd_movimentos);
    fprintf(estatisticas, "Numero de movimentos sem pontuar: %d\n", (qtd_movimentos - partida.pacman.pontos));
    fprintf(estatisticas, "Numero de colisoes com parede: %d\n", qtd_colisoes_parede);
    fprintf(estatisticas, "Numero de movimentos para baixo: %d\n", partida.qtdMovimentoS);
    fprintf(estatisticas, "Numero de movimentos para cima: %d\n", partida.qtdMovimentoW);
    fprintf(estatisticas, "Numero de movimentos para esquerda: %d\n", partida.qtdMovimentoA);
    fprintf(estatisticas, "Numero de movimentos para direita: %d\n", partida.qtdMovimentoD);

    fclose(estatisticas);
}

void GeraTrilha(char * argv[], tPartida partida){
    char caminho[1000];

    sprintf(caminho,"%s/saida/trilha.txt",argv[1]);

    FILE * trilha = fopen(caminho,"w");

    int i, j;
    for(i=0; i<partida.mapa.linha; i++){
        for(j=0; j<partida.mapa.coluna; j++){
            if(partida.trilha[i][j] == -1){
                fprintf(trilha, "#");
            }
            else{
                fprintf(trilha, "%d", partida.trilha[i][j]);
            }
            if(j < (partida.mapa.coluna - 1)){
                fprintf(trilha, " ");
            }
        }
        fprintf(trilha, "\n");
    }

    fclose(trilha);
}

void RealizarJogo(tPartida partida, char * argv[]){
    char caminho[1000];

    sprintf(caminho,"%s/saida/resumo.txt",argv[1]);

    FILE * resumo = fopen(caminho,"w");

    int i, qtd_comida_inicial, qtd_colisoes_parede = 0, qtd_movimentos;

    qtd_comida_inicial = RetornaQtdComida(partida.mapa);

    for(i=0; i<partida.mapa.qtd_movimentos; i++){
        scanf("%c", &partida.jogada);
        scanf("%*c");

        partida = MovimentaFantasma(partida);
        if(partida.bateuFantasma == 0){
            partida = MovimentaPacMan(partida);
            partida.trilha[partida.pacman.linha][partida.pacman.coluna] = i+1;
        }
        
        ImprimeEstadoAtual(partida.mapa, partida.jogada, partida.pacman.pontos);

        if(partida.pegouComida){
            fprintf(resumo, "Movimento %d (%c) pegou comida\n", i+1, partida.jogada);
            partida.pegouComida = 0;
        }
        if(partida.bateuFantasma){
            fprintf(resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", i+1, partida.jogada);
        }
        if(partida.colidiuParede){
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", i+1, partida.jogada);
            partida.colidiuParede = 0;
            qtd_colisoes_parede++;
        }
        if(qtd_comida_inicial == partida.pacman.pontos){
            break;
        }
        if(partida.bateuFantasma){
            break;
        }
    }

    if(i == partida.mapa.qtd_movimentos){
        qtd_movimentos = i;
    }else{
        qtd_movimentos = i+1;
    }

    fclose(resumo);

    GeraEstatisticas(argv, partida, qtd_colisoes_parede, qtd_movimentos);
    GeraTrilha(argv, partida);

    if(qtd_comida_inicial == partida.pacman.pontos){
        ImprimeVitoria(partida.pacman.pontos);
    }else{
        ImprimeDerrota(partida.pacman.pontos);
    }

}

int main(int argc, char * argv[]){
    tPartida partida;

    partida = InicializarJogo(argv);

    RealizarJogo(partida, argv);


    return 0;
}