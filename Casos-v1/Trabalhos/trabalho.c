#include <stdio.h>

//Criando os tipos necessarios para o jogo
typedef struct 
{
    int linha;
    int coluna;
}tTunel;

typedef struct 
{
    int linha;
    int coluna;
    int qtd_movimentos;
    char tabuleiro[40][100];
    char comida[40][100];
    int bateuFantasma; // 0 se não bateu, 1 se bateu
    int colidiuFantasmaParede; // 0 se o fantasma não colidiu, 1 se colidiu
    int colidiuParede; // 0 se o pacman não colidiu, 1 se colidiu 
    int pegouComida; // 0 se o pacman não pegou, 1 se pegou
    int entrouTunel1; // 0 se o pacman não entrou, 1 se entrou
    int entrouTunel2; // 0 se o pacman não entrou, 1 se entrou
    tTunel tunel1;
    tTunel tunel2;
}tMapa;

typedef struct 
{
    int linhaInicial;
    int colunaInicial;
    int linha;
    int coluna;
    int pontos;
}tPacMan;

typedef struct 
{
    int existe; // O se não existe, 1 se existe
    int linhaInicial;
    int colunaInicial;
    int linha;
    int coluna;
    int direcao; // 0 direção inicial, 1 direção contrária 
}tFantasma;

typedef struct 
{
    int qtdMovimentoW;
    int qtdMovimentoS;
    int qtdMovimentoD;
    int qtdMovimentoA;
    int qtdColisoesParede;
    int qtdMovimentos;
}tEstatisticas;

typedef struct 
{
    char tipo;
    int qtdMovimento;
    int qtdPegouComida;
    int qtdColisoesParede;
}tMovimento;


typedef struct 
{
    tMovimento movimentoW;
    tMovimento movimentoS;
    tMovimento movimentoD;
    tMovimento movimentoA;
}tRanking;


typedef struct 
{
    tMapa mapa;
    tPacMan pacman;
    tFantasma fantasmaB;
    tFantasma fantasmaP;
    tFantasma fantasmaI;
    tFantasma fantasmaC;
    char jogada;
    tEstatisticas estatisticas;
    tRanking ranking;
    int trilha[40][100];
}tPartida;

//Funcao para ler as entradas dadas em mapa.txt
tMapa LeMapa(char * argv[]){
    tMapa mapa;
    char caminho[1000];

    sprintf(caminho,"%s/mapa.txt",argv[1]);

    FILE * entrada = fopen(caminho,"r");

    if (entrada == NULL){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
    }

    fscanf(entrada, "%d %d %d\n", &mapa.linha, &mapa.coluna, &mapa.qtd_movimentos);

    int i, j, qualTunel = 0;
    for(i=0; i<mapa.linha; i++){
        for(j=0; j<mapa.coluna; j++){
            fscanf(entrada, "%c", &mapa.tabuleiro[i][j]);

            //Inicializando posicoes das comidas
            if(mapa.tabuleiro[i][j] == '*'){
                mapa.comida[i][j] = '*';
            }else{
                mapa.comida[i][j] = ' ';
            }

            //Inicializando posicoes dos tuneis
            if(mapa.tabuleiro[i][j] == '@' && qualTunel == 0){
                mapa.tunel1.linha = i;
                mapa.tunel1.coluna = j;
                qualTunel++;
            }else if(mapa.tabuleiro[i][j] == '@' && qualTunel){
                mapa.tunel2.linha = i;
                mapa.tunel2.coluna = j;
            }
        }
        fscanf(entrada, "%*c");
    }

    fclose(entrada);

    mapa.bateuFantasma = 0;
    mapa.colidiuFantasmaParede = 0;
    mapa.colidiuParede = 0;
    mapa.pegouComida = 0;
    mapa.entrouTunel1 = 0;
    mapa.entrouTunel2 = 0;

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

int EhPacMan(char peca){
    return peca == '>';
}

int EhFantasma(char peca){
    return peca == 'B' || peca == 'P' || peca == 'I' || peca == 'C';
}

int EhTunel(char peca){
    return peca == '@';
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
                pacman.linhaInicial = i;
                pacman.colunaInicial = j;
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
                fantasma.linhaInicial = i;
                fantasma.colunaInicial = j;
                fantasma.existe = 1;

                return fantasma;
            }
        }
    }

    return fantasma;
}

tEstatisticas InicializaEstatisticas(tEstatisticas estatisticas){
    estatisticas.qtdMovimentoW = 0;
    estatisticas.qtdMovimentoS = 0;
    estatisticas.qtdMovimentoD = 0;
    estatisticas.qtdMovimentoA = 0;
    estatisticas.qtdColisoesParede = 0;
    estatisticas.qtdMovimentos = 0;

    return estatisticas;
}

tMovimento InicializaMovimento(char tipo){
    tMovimento movimento;

    movimento.tipo = tipo;
    movimento.qtdMovimento = 0;
    movimento.qtdPegouComida = 0;
    movimento.qtdColisoesParede = 0;

    return movimento;
}

tRanking InicializaRanking(){
    tRanking ranking;

    ranking.movimentoW = InicializaMovimento('w');
    ranking.movimentoS = InicializaMovimento('s');
    ranking.movimentoD = InicializaMovimento('d');
    ranking.movimentoA = InicializaMovimento('a');
    

    return ranking;
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
    partida.estatisticas = InicializaEstatisticas(partida.estatisticas);
    partida.ranking = InicializaRanking();
    
    //Inicializa Trilha com todas as posições como -1, exceto posição inicial do pacman (0)
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

tFantasma MexeFantasma(tFantasma fantasma, char direcao){

    switch(direcao){
        case 'w':
            fantasma.linha = fantasma.linhaInicial - 1;
            break;
        case 's':
            fantasma.linha = fantasma.linhaInicial + 1;
            break;
        case 'd':
            fantasma.coluna = fantasma.colunaInicial + 1;
            break;
        case 'a':
            fantasma.coluna = fantasma.colunaInicial - 1;
            break;
        default:
            break; 
    }

    return fantasma;
}

tMapa AtualizaMapaFantasma(tFantasma fantasma, tMapa mapa, char tipo, char direcao, char jogada){
    //Verifica se a posição para a qual o fantasma está indo é comida ou pacman ou espaco
    if(EhComida(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) ||  EhPacMan(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) || 
    mapa.tabuleiro[fantasma.linha][fantasma.coluna] == ' ' || EhTunel(mapa.tabuleiro[fantasma.linha][fantasma.coluna])){

        //Se for o pacman, verifica se ele está vindo de encontro
        if(EhPacMan(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) && direcao == 'w' && jogada == 's' ||
        EhPacMan(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) && direcao == 's' && jogada == 'w' ||
        EhPacMan(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) && direcao == 'd' && jogada == 'a' ||
        EhPacMan(mapa.tabuleiro[fantasma.linha][fantasma.coluna]) && direcao == 'a' && jogada == 'd'){
            mapa.bateuFantasma = 1;
        }
        mapa.tabuleiro[fantasma.linha][fantasma.coluna] = tipo;

        //Verifica se o fantasma estava em cima de uma comida ou de um tunel
        if(EhComida(mapa.comida[fantasma.linhaInicial][fantasma.colunaInicial])){
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = '*';
        }else if(fantasma.linhaInicial == mapa.tunel1.linha && fantasma.colunaInicial == mapa.tunel1.coluna ||
        fantasma.linhaInicial == mapa.tunel2.linha && fantasma.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = '@';
        }else{
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = ' ';
        }
    }

    //Verifica se a posição para a qual o fantasma está indo é uma parede
    else if(mapa.tabuleiro[fantasma.linha][fantasma.coluna] == '#'){

        if(direcao == 'w') mapa.tabuleiro[fantasma.linhaInicial+1][fantasma.colunaInicial] = tipo;
        if(direcao == 's') mapa.tabuleiro[fantasma.linhaInicial-1][fantasma.colunaInicial] = tipo;
        if(direcao == 'd') mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial-1] = tipo;
        if(direcao == 'a') mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial+1] = tipo;

        //Verifica se o fantasma estava em cima de uma comida ou de um tunel
        if(EhComida(mapa.comida[fantasma.linhaInicial][fantasma.colunaInicial])){
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = '*';
        }else if(fantasma.linhaInicial == mapa.tunel1.linha && fantasma.colunaInicial == mapa.tunel1.coluna ||
        fantasma.linhaInicial == mapa.tunel2.linha && fantasma.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = '@';
        }else{
            mapa.tabuleiro[fantasma.linhaInicial][fantasma.colunaInicial] = ' ';
        }
        mapa.colidiuFantasmaParede = 1; // muda a direcao do fantasma
    }

    return mapa;
}

tFantasma AtualizaPosicaoFantasma(tFantasma fantasma){
    fantasma.linhaInicial = fantasma.linha;
    fantasma.colunaInicial = fantasma.coluna;

    return fantasma;
}

tPartida MovimentaFantasma(tPartida partida){

    //Verifica se o Fantasma B existe, se existe movimenta ele
    if(partida.fantasmaB.existe){

        //Movimenta o Fantasma B de acordo com sua direção inicial
        if(partida.fantasmaB.direcao == 0){
            partida.fantasmaB = MexeFantasma(partida.fantasmaB, 'a');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaB, partida.mapa, 'B', 'a', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaB.coluna = partida.fantasmaB.colunaInicial + 1;
                partida.fantasmaB.direcao = 1;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaB = AtualizaPosicaoFantasma(partida.fantasmaB);
        }

        //Movimenta o Fantasma B de acordo com sua direção contrária
        else if(partida.fantasmaB.direcao){
            partida.fantasmaB = MexeFantasma(partida.fantasmaB, 'd');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaB, partida.mapa, 'B', 'd', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaB.coluna = partida.fantasmaB.colunaInicial - 1;
                partida.fantasmaB.direcao = 0;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaB = AtualizaPosicaoFantasma(partida.fantasmaB);
        }
    }

    //Verifica se o Fantasma P existe, se existe movimenta ele
    if(partida.fantasmaP.existe){

        if(partida.fantasmaP.direcao == 0){
            partida.fantasmaP = MexeFantasma(partida.fantasmaP, 'w');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaP, partida.mapa, 'P', 'w', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaP.linha = partida.fantasmaP.linhaInicial + 1;
                partida.fantasmaP.direcao = 1;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaP = AtualizaPosicaoFantasma(partida.fantasmaP);
        }

        else if(partida.fantasmaP.direcao){
            partida.fantasmaP = MexeFantasma(partida.fantasmaP, 's');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaP, partida.mapa, 'P', 's', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaP.linha = partida.fantasmaP.linhaInicial - 1;
                partida.fantasmaP.direcao = 0;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaP = AtualizaPosicaoFantasma(partida.fantasmaP);
        }
    }

    //Verifica se o Fantasma I existe, se existe movimenta ele
    if(partida.fantasmaI.existe){

        if(partida.fantasmaI.direcao == 0){
            partida.fantasmaI = MexeFantasma(partida.fantasmaI, 's');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaI, partida.mapa, 'I', 's', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaI.linha = partida.fantasmaI.linhaInicial - 1;
                partida.fantasmaI.direcao = 1;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaI = AtualizaPosicaoFantasma(partida.fantasmaI);
        }

        else if(partida.fantasmaI.direcao){
            partida.fantasmaI = MexeFantasma(partida.fantasmaI, 'w');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaI, partida.mapa, 'I', 'w', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaI.linha = partida.fantasmaI.linhaInicial + 1;
                partida.fantasmaI.direcao = 0;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaI = AtualizaPosicaoFantasma(partida.fantasmaI);
        }
    }

    //Verifica se o Fantasma C existe, se existe movimenta ele
    if(partida.fantasmaC.existe){

        if(partida.fantasmaC.direcao == 0){
            partida.fantasmaC = MexeFantasma(partida.fantasmaC, 'd');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaC, partida.mapa, 'C', 'd', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaC.coluna = partida.fantasmaC.colunaInicial - 1;
                partida.fantasmaC.direcao = 1;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaC = AtualizaPosicaoFantasma(partida.fantasmaC);
        }

        else if(partida.fantasmaC.direcao){
            partida.fantasmaC = MexeFantasma(partida.fantasmaC, 'a');
            partida.mapa = AtualizaMapaFantasma(partida.fantasmaC, partida.mapa, 'C', 'a', partida.jogada);
            if(partida.mapa.colidiuFantasmaParede){
                partida.fantasmaC.coluna = partida.fantasmaC.colunaInicial + 1;
                partida.fantasmaC.direcao = 0;
                partida.mapa.colidiuFantasmaParede = 0;
            }
            partida.fantasmaC = AtualizaPosicaoFantasma(partida.fantasmaC);
        }
    }

    return partida;
}

tMapa AtualizaMapaPacMan(tPacMan pacman, tMapa mapa){

    //Verifica se a posição para a qual o pacman esta indo é espaco
    if(mapa.tabuleiro[pacman.linha][pacman.coluna] == ' '){
        mapa.tabuleiro[pacman.linha][pacman.coluna] = '>';
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = ' ';
        }
        //Verifica se o pacman estava em cima de um tunel
        if(pacman.linhaInicial == mapa.tunel1.linha && pacman.colunaInicial == mapa.tunel1.coluna ||
        pacman.linhaInicial == mapa.tunel2.linha && pacman.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '@';
        }
    }

    //Verifica se a posição para a qual o pacman esta indo é comida
    else if(EhComida(mapa.tabuleiro[pacman.linha][pacman.coluna])){
        mapa.tabuleiro[pacman.linha][pacman.coluna] = '>';
        mapa.comida[pacman.linha][pacman.coluna] = ' ';
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = ' ';
        }
        //Verifica se o pacman estava em cima de um tunel
        if(pacman.linhaInicial == mapa.tunel1.linha && pacman.colunaInicial == mapa.tunel1.coluna ||
        pacman.linhaInicial == mapa.tunel2.linha && pacman.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '@';
        }
        mapa.pegouComida = 1;
    }

    //Verifica se a posição para a qual o pacman esta indo é fantasma
    else if(EhFantasma(mapa.tabuleiro[pacman.linha][pacman.coluna])){
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = ' ';
        }
        //Verifica se o pacman estava em cima de um tunel
        if(pacman.linhaInicial == mapa.tunel1.linha && pacman.colunaInicial == mapa.tunel1.coluna ||
        pacman.linhaInicial == mapa.tunel2.linha && pacman.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '@';
        }
        mapa.bateuFantasma = 1;
    }

    //Verifica se a posição para a qual o pacman esta indo é tunel 1
    else if(pacman.linha == mapa.tunel1.linha && pacman.coluna == mapa.tunel1.coluna){
        mapa.tabuleiro[mapa.tunel2.linha][mapa.tunel2.coluna] = '>';
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = ' ';
        }
        mapa.entrouTunel1 = 1;
    }
    
    //Verifica se a posição para a qual o pacman esta indo é tunel 2
    else if(pacman.linha == mapa.tunel2.linha && pacman.coluna == mapa.tunel2.coluna){
        mapa.tabuleiro[mapa.tunel1.linha][mapa.tunel1.coluna] = '>';
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = ' ';
        }
        mapa.entrouTunel2 = 1;
    }

    //Verifica se a posição para a qual o pacman esta indo é parede
    else if(mapa.tabuleiro[pacman.linha][pacman.coluna] == '#'){
        if(!EhFantasma(mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial])){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '>';
        }else{
            mapa.bateuFantasma = 1;
        }
        //Verifica se o pacman estava em cima de um tunel 1
        if(pacman.linhaInicial == mapa.tunel1.linha && pacman.colunaInicial == mapa.tunel1.coluna){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '@';
            mapa.tabuleiro[mapa.tunel2.linha][mapa.tunel2.coluna] = '>';
            mapa.entrouTunel1 = 1;
        }
        //Verifica se o pacman estava em cima de um tunel 2
        if(pacman.linhaInicial == mapa.tunel2.linha && pacman.colunaInicial == mapa.tunel2.coluna){
            mapa.tabuleiro[pacman.linhaInicial][pacman.colunaInicial] = '@';
            mapa.tabuleiro[mapa.tunel1.linha][mapa.tunel1.coluna] = '>';
            mapa.entrouTunel2 = 1;
        }
        mapa.colidiuParede = 1;
    }

    return mapa;
}

//Verifica se entrou no tunel e atualiza as posicoes
tPartida VerificaTunel(tPartida partida){
    if(partida.mapa.entrouTunel1){
        partida.pacman.linha = partida.mapa.tunel2.linha;
        partida.pacman.coluna = partida.mapa.tunel2.coluna;
        partida.mapa.entrouTunel1 = 0;
    } 
    if(partida.mapa.entrouTunel2){
        partida.pacman.linha = partida.mapa.tunel1.linha;
        partida.pacman.coluna = partida.mapa.tunel1.coluna;
        partida.mapa.entrouTunel2 = 0;
    }

    return partida;
}

tPacMan AtualizaPosicaoPacMan(tPacMan pacman){
    pacman.linhaInicial = pacman.linha;
    pacman.colunaInicial = pacman.coluna;

    return pacman;
}

tPartida MovimentaPacMan(tPartida partida){

    switch (partida.jogada)
    {

    case 'w':
        partida.estatisticas.qtdMovimentoW++;

        partida.pacman.linha = partida.pacman.linhaInicial - 1;
        partida.mapa = AtualizaMapaPacMan(partida.pacman, partida.mapa);
        if (partida.mapa.pegouComida){
            partida.ranking.movimentoW.qtdPegouComida++;
            partida.pacman.pontos++;
        } 
        if(partida.mapa.colidiuParede){
            partida.ranking.movimentoW.qtdColisoesParede++;
            partida.pacman.linha = partida.pacman.linhaInicial;
        } 
        partida = VerificaTunel(partida);
        partida.pacman = AtualizaPosicaoPacMan(partida.pacman);

        break;

    case 's':
        partida.estatisticas.qtdMovimentoS++;

        partida.pacman.linha = partida.pacman.linhaInicial + 1;
        partida.mapa = AtualizaMapaPacMan(partida.pacman, partida.mapa);
        if (partida.mapa.pegouComida){
            partida.ranking.movimentoS.qtdPegouComida++;
            partida.pacman.pontos++;
        } 
        if(partida.mapa.colidiuParede){
            partida.ranking.movimentoS.qtdColisoesParede++;
            partida.pacman.linha = partida.pacman.linhaInicial;
        }
        partida = VerificaTunel(partida);
        partida.pacman = AtualizaPosicaoPacMan(partida.pacman);

        break;

    case 'd':
        partida.estatisticas.qtdMovimentoD++;

        partida.pacman.coluna = partida.pacman.colunaInicial + 1;
        partida.mapa = AtualizaMapaPacMan(partida.pacman, partida.mapa);
        if (partida.mapa.pegouComida){
            partida.ranking.movimentoD.qtdPegouComida++;
            partida.pacman.pontos++;
        } 
        if(partida.mapa.colidiuParede){
            partida.ranking.movimentoD.qtdColisoesParede++;
            partida.pacman.coluna = partida.pacman.colunaInicial;
        }
        partida = VerificaTunel(partida);
        partida.pacman = AtualizaPosicaoPacMan(partida.pacman);

        break;

    case 'a':
        partida.estatisticas.qtdMovimentoA++;

        partida.pacman.coluna = partida.pacman.colunaInicial - 1;
        partida.mapa = AtualizaMapaPacMan(partida.pacman, partida.mapa);
        if (partida.mapa.pegouComida){
            partida.ranking.movimentoA.qtdPegouComida++;
            partida.pacman.pontos++;
        } 
        if(partida.mapa.colidiuParede){
            partida.ranking.movimentoA.qtdColisoesParede++;
            partida.pacman.coluna = partida.pacman.colunaInicial;
        }
        partida = VerificaTunel(partida);  
        partida.pacman = AtualizaPosicaoPacMan(partida.pacman);

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

void GeraEstatisticas(char * argv[], tEstatisticas stats, int pontos){
    char caminho[1000];

    sprintf(caminho,"%s/saida/estatisticas.txt",argv[1]);

    FILE * estatisticas = fopen(caminho,"w");

    fprintf(estatisticas, "Numero de movimentos: %d\n", stats.qtdMovimentos);
    fprintf(estatisticas, "Numero de movimentos sem pontuar: %d\n", (stats.qtdMovimentos - pontos));
    fprintf(estatisticas, "Numero de colisoes com parede: %d\n", stats.qtdColisoesParede);
    fprintf(estatisticas, "Numero de movimentos para baixo: %d\n", stats.qtdMovimentoS);
    fprintf(estatisticas, "Numero de movimentos para cima: %d\n", stats.qtdMovimentoW);
    fprintf(estatisticas, "Numero de movimentos para esquerda: %d\n", stats.qtdMovimentoA);
    fprintf(estatisticas, "Numero de movimentos para direita: %d\n", stats.qtdMovimentoD);

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

//Esta funcao retorna 1 se o movimento1 for melhor que o movimento2 e 0 caso contrario
int MelhorMovimento(tMovimento movimento1, tMovimento movimento2){
    if(movimento1.qtdPegouComida > movimento2.qtdPegouComida){
        return 1;
    }else if(movimento1.qtdPegouComida == movimento2.qtdPegouComida){
        if(movimento1.qtdColisoesParede < movimento2.qtdColisoesParede){
            return 1;
        }else if(movimento1.qtdColisoesParede == movimento2.qtdColisoesParede){
            if(movimento1.qtdMovimento > movimento2.qtdMovimento){
                return 1;
            }else if(movimento1.qtdMovimento == movimento2.qtdMovimento){
                if(movimento1.tipo < movimento2.tipo){
                    return 1;
                }
            }
        }
    }

    return 0;
}

void GeraRanking(char * argv[], tRanking ranking, tEstatisticas estatisticas){

    //Atribuindo qtd movimentos de cada tipo
    ranking.movimentoW.qtdMovimento = estatisticas.qtdMovimentoW;
    ranking.movimentoS.qtdMovimento = estatisticas.qtdMovimentoS;
    ranking.movimentoD.qtdMovimento = estatisticas.qtdMovimentoD;
    ranking.movimentoA.qtdMovimento = estatisticas.qtdMovimentoA;

    //Inicializando as variaveis que ao receber os movimentos
    tMovimento primeiro, segundo, terceiro, quarto;
    primeiro = InicializaMovimento('x');
    segundo = InicializaMovimento('x');
    terceiro = InicializaMovimento('x');
    quarto = InicializaMovimento('x');

    //Verifica a posicao do movimento W
    if(MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoW, ranking.movimentoA)){
        primeiro = ranking.movimentoW;
    }
    else if(!MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoW, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoW, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoW, ranking.movimentoA)){
        segundo = ranking.movimentoW;
    }
    else if(!MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoW, ranking.movimentoA) ||
    !MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoW, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoW, ranking.movimentoA)){
        terceiro = ranking.movimentoW;
    }
    else if(!MelhorMovimento(ranking.movimentoW, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoW, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoW, ranking.movimentoA)){
        quarto = ranking.movimentoW;
    }

    //Verifica a posicao do movimento S
    if(MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoS, ranking.movimentoA)){
        primeiro = ranking.movimentoS;
    }
    else if(!MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoS, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && !MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoS, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoS, ranking.movimentoA)){
        segundo = ranking.movimentoS;
    }
    else if(!MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && !MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoS, ranking.movimentoA) ||
    !MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoS, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && !MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoS, ranking.movimentoA)){
        terceiro = ranking.movimentoS;
    }
    else if(!MelhorMovimento(ranking.movimentoS, ranking.movimentoW) && !MelhorMovimento(ranking.movimentoS, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoS, ranking.movimentoA)){
        quarto = ranking.movimentoS;
    }

    //Verifica a posicao do movimento D
    if(MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && MelhorMovimento(ranking.movimentoD, ranking.movimentoA)){
        primeiro = ranking.movimentoD;
    }
    else if(!MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && MelhorMovimento(ranking.movimentoD, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && MelhorMovimento(ranking.movimentoD, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && !MelhorMovimento(ranking.movimentoD, ranking.movimentoA)){
        segundo = ranking.movimentoD;
    }
    else if(!MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && MelhorMovimento(ranking.movimentoD, ranking.movimentoA) ||
    !MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && !MelhorMovimento(ranking.movimentoD, ranking.movimentoA) ||
    MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && !MelhorMovimento(ranking.movimentoD, ranking.movimentoA)){
        terceiro = ranking.movimentoD;
    }
    else if(!MelhorMovimento(ranking.movimentoD, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoD, ranking.movimentoW)
    && !MelhorMovimento(ranking.movimentoD, ranking.movimentoA)){
        quarto = ranking.movimentoD;
    }

    //Verifica a posicao do movimento A
    if(MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoA, ranking.movimentoW)){
        primeiro = ranking.movimentoA;
    }
    else if(!MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoA, ranking.movimentoW) ||
    MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoA, ranking.movimentoW) ||
    MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoA, ranking.movimentoW)){
        segundo = ranking.movimentoA;
    }
    else if(!MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && MelhorMovimento(ranking.movimentoA, ranking.movimentoW) ||
    !MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoA, ranking.movimentoW) ||
    MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoA, ranking.movimentoW)){
        terceiro = ranking.movimentoA;
    }
    else if(!MelhorMovimento(ranking.movimentoA, ranking.movimentoS) && !MelhorMovimento(ranking.movimentoA, ranking.movimentoD)
    && !MelhorMovimento(ranking.movimentoA, ranking.movimentoW)){
        quarto = ranking.movimentoA;
    }

    //Abre e preenche o arquivo
    char caminho[1000];

    sprintf(caminho,"%s/saida/ranking.txt",argv[1]);

    FILE * rankingArq = fopen(caminho,"w");

    fprintf(rankingArq, "%c,%d,%d,%d\n", primeiro.tipo, primeiro.qtdPegouComida, primeiro.qtdColisoesParede, primeiro.qtdMovimento);
    fprintf(rankingArq, "%c,%d,%d,%d\n", segundo.tipo, segundo.qtdPegouComida, segundo.qtdColisoesParede, segundo.qtdMovimento);
    fprintf(rankingArq, "%c,%d,%d,%d\n", terceiro.tipo, terceiro.qtdPegouComida, terceiro.qtdColisoesParede, terceiro.qtdMovimento);
    fprintf(rankingArq, "%c,%d,%d,%d\n", quarto.tipo, quarto.qtdPegouComida, quarto.qtdColisoesParede, quarto.qtdMovimento);

    fclose(rankingArq);
}

void RealizarJogo(tPartida partida, char * argv[]){

    //Abre arquivo para o resumo
    char caminho[1000];

    sprintf(caminho,"%s/saida/resumo.txt",argv[1]);

    FILE * resumo = fopen(caminho,"w");

    //Inicializa variaveis uteis
    int i, qtd_comida_inicial;

    qtd_comida_inicial = RetornaQtdComida(partida.mapa);

    //Comeca o jogo
    for(i=0; i<partida.mapa.qtd_movimentos; i++){
        scanf("%c", &partida.jogada);
        scanf("%*c");

        partida = MovimentaFantasma(partida);

        //Se o pacman não morreu, movimenta ele
        if(partida.mapa.bateuFantasma == 0){
            partida = MovimentaPacMan(partida);

            //se o pacman ainda não morreu após ele se movimentar, atualiza trilha
            if(!partida.mapa.bateuFantasma) partida.trilha[partida.pacman.linha][partida.pacman.coluna] = i+1;
        }else{
            if(partida.jogada == 'w') partida.estatisticas.qtdMovimentoW++;
            if(partida.jogada == 's') partida.estatisticas.qtdMovimentoS++;
            if(partida.jogada == 'd') partida.estatisticas.qtdMovimentoD++;
            if(partida.jogada == 'a') partida.estatisticas.qtdMovimentoA++;
        }
        
        ImprimeEstadoAtual(partida.mapa, partida.jogada, partida.pacman.pontos);

        //Atualiza o resumo
        if(partida.mapa.pegouComida){
            fprintf(resumo, "Movimento %d (%c) pegou comida\n", i+1, partida.jogada);
            partida.mapa.pegouComida = 0;
        }
        if(partida.mapa.bateuFantasma){
            fprintf(resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", i+1, partida.jogada);
        }
        if(partida.mapa.colidiuParede){
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", i+1, partida.jogada);
            partida.mapa.colidiuParede = 0;
            partida.estatisticas.qtdColisoesParede++;
        }

        //Se ganhou, para de ler as jogadas
        if(qtd_comida_inicial == partida.pacman.pontos){
            break;
        }
        //Se perdeu por bater no fantasma, para de ler as jogadas
        if(partida.mapa.bateuFantasma){
            break;
        }
    }

    //Define a quantidade de movimentos realizados no jogo
    if(i == partida.mapa.qtd_movimentos){
        partida.estatisticas.qtdMovimentos = i;
    }else{
        partida.estatisticas.qtdMovimentos = i+1;
    }

    fclose(resumo); 

    //Gera arquivos
    GeraEstatisticas(argv, partida.estatisticas, partida.pacman.pontos);
    GeraTrilha(argv, partida);
    GeraRanking(argv, partida.ranking, partida.estatisticas);

    //Verifica o resultado final
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