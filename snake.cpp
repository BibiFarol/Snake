#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <string>
#include <sys/time.h> //biblioteca para leitura do tempo

#define TEMPOMOVIMENTACAO 200

using namespace std;
typedef struct
{
    int x, y;     // coordenadas
    int vida;     // numero de vidas
    float pontos; // pontos que o personagem ainda tem
    int macas;
} Personagem; // este eh o nome do novo tipo criado

const char BLOCO = '#';
const char MACA = '@';

// Tamanho da área util
const int COLUNAS = 31;
const int LINHAS = 10;
const int MAX_MACA = 10;

// Quantas linhas desconsiderar do titulo da fase
const int TITULO_OFFSET = 5;

void SetCor(int BackGroundColor, int ForeGroundColor)
{
    /*

    Color      Background    Foreground
    ---------------------------------------------
    Black            0           0
    Blue             1           1
    Green            2           2
    Cyan             3           3
    Red              4           4
    Magenta          5           5
    Brown            6           6
    White            7           7
    Gray             -           8
    Intense Blue     -           9
    Intense Green    -           10
    Intense Cyan     -           11
    Intense Red      -           12
    Intense Magenta  -           13
    Yellow           -           14
    Intense White    -           15
    */

    int color = 16 * BackGroundColor + ForeGroundColor;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y)
{
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        (COORD){x - 1, y - 1});
}

void apaga(char **mapa, int x, int y)
{
    gotoxy(x, y);

    // Atualiza a posicao do snake no mapa
    mapa[y - 1][x - 1] = ' ';
    cout << " ";
}

void desenha(char **mapa, int x, int y, char p, bool vul)
{
    gotoxy(x, y);

    // Atualiza a posicao do snake no mapa
    mapa[y - 1][x - 1] = 'S';
    if (p == 'S')
    {
        SetCor(0, 2);
    }

    cout << p;
}

void imprimeMapa(char **mapa)
{
    for (int j = 0; j < 17; j++)
    {
        for (int i = 0; i < 34; i++)
        {
            switch (mapa[j][i])
            {
            case '#':
                SetCor(9, 1);
                break;

            case 'o':
                SetCor(0, 4);
                break;

            case '@':
                SetCor(0, 12);
                break;

            case '_':
            case '|':
            case '/':
            case '\\':
                SetCor(0, 10);
                break;

            default:
                SetCor(0, 7);
                break;
            }
            cout << mapa[j][i];
        }
        cout << endl;
    }
    gotoxy(1, 1);
    SetCor(0, 7);
}

bool lerMapaArq(char **mapa, string nomeArq)
{
    string linha;                 // Variável que receberá a linha a ser lida
    ifstream meuArquivo(nomeArq); // Declara a variável e abre o arquivo exemplo.txt
    int i = 0;

    if (meuArquivo.is_open()) // Verifica que o arquivo foi aberto
    {
        while (getline(meuArquivo, linha)) // A condição de parada do while é q (EndOf File)
        {
            strcpy(mapa[i++], linha.c_str()); // Imprime a linha na tela
        }
        meuArquivo.close(); // Fecha o arquivo
        return true;
    }
    else
    {
        cout << "Nao foi possivel abrir o arquivo\n\n"; // Caso não tenha sido aberto o arquivo imprime a mensagem de erro.
        return false;
    }
}

void criarBlocoAleatorio(char **mapa, int qtde)
{
    int x, y;
    int i = 0;

    while (i < qtde)
    {
        // numero inteiro entre 1 e COLUNAS
        x = (rand() % COLUNAS) + 1;

        // numero inteiro entre 1 e LINHAS
        y = ((rand() % LINHAS) + 1) + TITULO_OFFSET;

        // Espaço está vazio no mapa ?
        if (mapa[y][x] == ' ')
        {
            mapa[y][x] = BLOCO;

            // imprime o bloco
            SetCor(9, 1);
            gotoxy(x + 1, y + 1);
            cout << BLOCO;

            i++;
        }
    }
}

//
// Spawna uma nova maçã
//
void spawMaca(char **mapa)
{
    int x, y;

    while (true)
    {
        // numero inteiro entre 1 e COLUNAS
        x = (rand() % COLUNAS) + 1;

        // numero inteiro entre 1 e LINHAS
        y = ((rand() % LINHAS) + 1) + TITULO_OFFSET;

        // Espaço está vazio no mapa ?
        if (mapa[y][x] == ' ')
        {
            mapa[y][x] = MACA;

            // imprime a maca
            SetCor(0, 12);
            gotoxy(x + 1, y + 1);
            cout << MACA;
            break;
        }
    }
}

int main()
{

    struct timeval inicio, atual, iSnake; // estruturas para leitura do tempo pelo gettimeofsday
    long double tempo = 0.0;              // variavel para armazenar o tempo de execução.
    long double tSnake = 0.0;
    char placar[80];
    int accel_x = 0;
    int accel_y = 0;

    int x, y;

    srand(time(NULL)); // inicializa gerador de variáveis aleatorias

    char **mapa = new char *[17]; // É preciso fazer isso para passar por parâmetro para a função alterar o valor desta variável
    for (int i = 0; i < 17; i++)
    {
        mapa[i] = new char[34];
    }

    // Abre o arquivo e le todo o mapa para a matriz **mapa;
    if (lerMapaArq(mapa, "mapa.txt"))
    {
        // valores iniciais das variaveis do personagem
        Personagem p1;
        p1.x = 16;
        p1.y = 12;
        p1.vida = 1;
        p1.pontos = 0;
        p1.macas = 0;

        CONSOLE_CURSOR_INFO ConCurInf; // este é o objeto que configura o cursor
        ConCurInf.dwSize = 10;         // este é o tamanho do cursor, que vai de 0% a 100%
        ConCurInf.bVisible = false;    // este atributo o deixa visivel ou não
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConCurInf);
        // setlocale(LC_ALL,"");
        // system("chcp65001");
        // system("cls");

        imprimeMapa(mapa);

        desenha(mapa, p1.x, p1.y, 'S', false); // desenha o personagem na posição inicial.

        criarBlocoAleatorio(mapa, 5);

        spawMaca(mapa);

        // Inicializa os cronometros, do inicio do jogo para os pontos e do tempo dos fantasmas para a movimentação.
        gettimeofday(&inicio, NULL);
        gettimeofday(&iSnake, NULL);
        char ch = ' ';

        while (true)
        {
            gettimeofday(&atual, NULL);

            // Movimentação do snake
            if (kbhit())
            {
                ch = getch();
            }
            tSnake = ((atual.tv_sec + atual.tv_usec / 1000000.0) - (iSnake.tv_sec + iSnake.tv_usec / 1000000.0)) * 1000.0;
            if (tSnake >= TEMPOMOVIMENTACAO)
            {
                gettimeofday(&iSnake, NULL);

                x = p1.x;
                y = p1.y;

                switch (ch)
                {
                case 'W':
                case 'w':
                    accel_y = -1;
                    accel_x = 0;
                    break;
                case 'A':
                case 'a':
                    accel_x = -1;
                    accel_y = 0;
                    break;
                case 'D':
                case 'd':
                    accel_x = 1;
                    accel_y = 0;
                    break;
                case 'S':
                case 's':
                    accel_y = 1;
                    accel_x = 0;
                    break;
                default:
                    break;
                }

                x += accel_x;
                y += accel_y;

                apaga(mapa, p1.x, p1.y);

                // x e y é a nova posicao do snake

                if (mapa[y - 1][x - 1] == '#')
                {
                    gotoxy(15, 20);
                    cout << "MORREU!";
                    gotoxy(15, 25);
                    return 0;
                }

                // As maçãs, envolvem os corpos nus, neste rio que corre...
                if (mapa[y - 1][x - 1] == '@')
                {
                    mapa[y - 1][x - 1] = ' ';
                    // p1.pontos += 5;

                    // Uma nova maca surgira do nada
                    if (++p1.macas < MAX_MACA)
                        spawMaca(mapa);
                }

                p1.x = x;
                p1.y = y;

                desenha(mapa, p1.x, p1.y, 'S', false);

                // Placar
                SetCor(0, 6);
                gotoxy(15, 18);
                sprintf(placar, "Macas [%d / %d]", p1.macas, MAX_MACA);
                cout << placar;

                if (p1.macas >= MAX_MACA)
                {
                    // system("cls");

                    gotoxy(15, 20);
                    cout << "WINNER!!";
                    // system("@echo off");
                    // system("pause >nul 2>&1");
                    // cin.clear();
                    gotoxy(15, 25);
                    return 0;
                }
            }

            // Sleep(50);
        }
    }
    // Se não encontrar o mapa ou não conseguir ler, exibe o erro.
    else
    {
        cout << " Desculpe, mas nao encontrei o arquivo do mapa.txt";
    }
}
