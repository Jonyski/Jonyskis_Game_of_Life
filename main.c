/*******************************************************************************************
 *
 *   raylib - game: Conway's Game of Life
 *
 *   Sample game developed by Jonyski
 *
 *   This game has been created using raylib v5.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h
 *for details)
 *
 ********************************************************************************************/

#include "raylib.h"
#include <math.h>
#include <stdbool.h>

typedef enum { LIGHT, DARK } TemaFonte;

//------------------------------------------------------------------------------------
// Variáveis globais
//------------------------------------------------------------------------------------
#define ESPACO_EXTRA 10
#define LARGURA_UNIVERSO 50 + ESPACO_EXTRA * 2
#define ALTURA_UNIVERSO 30 + ESPACO_EXTRA * 2
const int largura_janela = 1000;
const int altura_janela = 600;
const int tamanho_celula = 20;
bool universo[ALTURA_UNIVERSO][LARGURA_UNIVERSO] = {
    0};               // a canvas containing our cells
bool rodando = false; // if the simulation of the game of life is running or not
float periodo_universo = 0.2f; // the update time of the universo
float timer_universo = 0.0f;
float timer_teclagem = 0.0f;
float cooldown_periodo = 0.12f;
Color cor_celula_morta = {255, 255, 255, 255};
Color cor_celula_viva = {0, 0, 0, 255};
Color cor_texto_forte = {0, 0, 0, 50};
Color cor_texto_fraco = {0, 0, 0, 32};
TemaFonte temas_fonte[10] = {DARK,  DARK,  DARK, DARK, LIGHT,
                             LIGHT, LIGHT, DARK, DARK, DARK};
Color paletas[10][2] = {
    {(Color){255, 255, 255, 255}, (Color){0, 0, 0, 255}},        // vida
    {(Color){231, 6, 55, 255}, (Color){255, 230, 174, 255}},     // solar
    {(Color){149, 245, 249, 255}, (Color){219, 65, 106, 255}},   // sonho
    {(Color){255, 58, 114, 255}, (Color){2, 31, 83, 255}},       // cereja
    {(Color){22, 23, 26, 255}, (Color){123, 115, 222, 255}},     // noite
    {(Color){17, 17, 17, 255}, (Color){121, 255, 139, 255}},     // hacker
    {(Color){3, 26, 65, 255}, (Color){96, 212, 255, 255}},       // oceano
    {(Color){180, 130, 214, 255}, (Color){57, 45, 126, 255}},    // mirtilo
    {(Color){226, 178, 143, 255}, (Color){148, 76, 74, 255}},    // amora
    {(Color){243, 161, 166, 255}, (Color){255, 212, 212, 255}}}; // pêssego

//------------------------------------------------------------------------------------
// Prototipos de funções
//------------------------------------------------------------------------------------
void inverte_celula(int x, int y); // switches a cell
void trata_click();
void atualiza_universo();  // simulates 1 step in the game of life
void renderiza_universo(); // renders the cells
void trata_teclagem();
int conta_vizinhos(bool universo[ALTURA_UNIVERSO][LARGURA_UNIVERSO], int x,
                   int y);
void trocaTemaFonte(TemaFonte tema);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  InitWindow(largura_janela, altura_janela, "Jonyski's Game of Life");
  SetTargetFPS(60);
  ClearBackground(cor_celula_morta);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(cor_celula_morta);
    DrawText("aperte ESPAÇO para rodar", 320, 288, 24, cor_texto_forte);
    DrawText("aperte 0 - 9 para mudar a paleta de cores", 250, 322, 22,
             cor_texto_fraco);
    DrawText(TextFormat("período do universo: %02.02fs", periodo_universo), 340,
             10, 20, cor_texto_forte);

    timer_teclagem += GetFrameTime();
    trata_teclagem();

    if (rodando) {
      timer_universo += GetFrameTime();
      if (timer_universo >= periodo_universo) {
        atualiza_universo();
        timer_universo = 0.0f;
      }
    }

    trata_click();
    renderiza_universo();

    EndDrawing();
  }
  CloseWindow();

  return 0;
}

void trata_click() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 posicao_click = GetMousePosition();
    // finding the indexes of the cell to switch
    int x = floor(posicao_click.x / tamanho_celula) + ESPACO_EXTRA;
    int y = floor(posicao_click.y / tamanho_celula) + ESPACO_EXTRA;
    inverte_celula(x, y);
  }
}

void inverte_celula(int x, int y) { universo[y][x] = !universo[y][x]; }

void atualiza_universo() {
  // criando uma cópia do universo, para podermos alterar o original sem afetar
  // o estado passado
  bool universo_paralelo[ALTURA_UNIVERSO][LARGURA_UNIVERSO];
  for (int i = 0; i < ALTURA_UNIVERSO; i++) {
    for (int j = 0; j < LARGURA_UNIVERSO; j++) {
      universo_paralelo[i][j] = universo[i][j];
    }
  }

  // atualizando o universo real
  int vizinhos = 0;
  for (int i = 0; i < ALTURA_UNIVERSO; i++) {
    for (int j = 0; j < LARGURA_UNIVERSO; j++) {
      vizinhos = conta_vizinhos(universo_paralelo, j, i);
      // aplicando as regras do jogo da vida
      if (universo[i][j] && (vizinhos < 2 || vizinhos > 3))
        universo[i][j] = false;
      else if (vizinhos == 3)
        universo[i][j] = true;
    }
  }
}

void renderiza_universo() {
  for (int i = ESPACO_EXTRA; i < ALTURA_UNIVERSO - ESPACO_EXTRA; i++) {
    for (int j = ESPACO_EXTRA; j < LARGURA_UNIVERSO - ESPACO_EXTRA; j++) {
      if (universo[i][j]) {
        int x = j - ESPACO_EXTRA;
        int y = i - ESPACO_EXTRA;
        DrawRectangle(x * tamanho_celula, y * tamanho_celula, tamanho_celula,
                      tamanho_celula, cor_celula_viva);
      }
    }
  }
}

void trata_teclagem() {
  int tecla = GetKeyPressed();
  switch (tecla) {
  // key that runs/pauses the simulation
  case KEY_SPACE:
    rodando = rodando ? false : true;
    break;
  // keys that change the simulation speed
  case KEY_MINUS:
    periodo_universo += 0.02;
    timer_teclagem = -0.4f;
    break;
  case KEY_EQUAL:
    if (periodo_universo - 0.02 >= 0.02)
      periodo_universo -= 0.02;
    timer_teclagem = -0.4f;
    break;
  }

  if (tecla >= KEY_ZERO && tecla <= KEY_NINE) {
    int idx = tecla - KEY_ZERO;
    cor_celula_morta = paletas[idx][0];
    cor_celula_viva = paletas[idx][1];
    trocaTemaFonte(temas_fonte[idx]);
  }

  if (timer_teclagem >= cooldown_periodo) {
    if (IsKeyDown(KEY_MINUS))
      periodo_universo += 0.02;
    if (IsKeyDown(KEY_EQUAL))
      if (periodo_universo - 0.02 >= 0.02)
        periodo_universo -= 0.02;
    timer_teclagem = 0.0f;
  }
}

int conta_vizinhos(bool universo[ALTURA_UNIVERSO][LARGURA_UNIVERSO], int x,
                   int y) {
  int numero_vizinhos = 0;

  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0)
        continue;

      int nx = x + j;
      int ny = y + i;

      // Verifica se o vizinho está dentro dos limites da grade
      if (nx >= 0 && nx < LARGURA_UNIVERSO && ny >= 0 && ny < ALTURA_UNIVERSO) {
        numero_vizinhos += universo[ny][nx];
      }
    }
  }
  return numero_vizinhos;
}

void trocaTemaFonte(TemaFonte tema) {
  switch (tema) {
  case DARK:
    cor_texto_forte = (Color){0, 0, 0, 50};
    cor_texto_fraco = (Color){0, 0, 0, 32};
    break;
  case LIGHT:
    cor_texto_forte = (Color){255, 255, 255, 50};
    cor_texto_fraco = (Color){255, 255, 255, 32};
  }
}
