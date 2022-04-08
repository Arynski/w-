#ifndef STRUKTURY_H
#define STRUKTURY_H


#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>

#define LEWO 260
#define PRAWO 261
#define GORA 259
#define DOL 258

/*rozmiar planszy*/
extern int PLANSZAX; /*40*/
extern int PLANSZAY; /*20*/
extern int TRUDNOSC; //1 - trudny, 2 - sredni, 3 - latwyint PLANSZAX = 40; /*40*/

/*wektor i punkt*/
typedef struct {
  int x;
  int y;
} wektor;

/*pojedyncza czesc weza*/
typedef struct {
  bool czyGlowa;
  wektor pozycja;
} czescWeza;

/*dynamiczna tablica*/
typedef struct {
  czescWeza* tablica;
  int rozmiar; /*pamiec zaalokowana*/
  int zuzyte; /*zuzyta pamiec*/
} dyntab;


typedef struct {
  dyntab cialo;
  int dlugosc;
  wektor kurs;
  char cialoWeza;
  char glowaWeza;
} waz;

void alokujDyntab(dyntab* tab);
void dodajDyntab(dyntab* tab, czescWeza* x);
void zwolnijDyntab(dyntab* tab);
void wyswietlPlansze();
waz* stworzWeza(char cialo, char glowa);
czescWeza* stworzCzescWeza(int y, int x, bool _czyGlowa);
void wyswietlWeza(waz* postac);
bool czyPorazka(waz* waz);
void ustawKierunek(waz* waz, int wejscie);
void zmienPozycje(int y, int x, czescWeza* cz);
bool aktualizujWeza(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2], int kierunek, bool* bylDodany); /*true jesli nie wchodzi sam w siebie*/
void formatujCzas(int y, int x, int sekundy);
void dodajDoWeza(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2]);
void wyswietlPlanszeBool(bool plansza[PLANSZAY][PLANSZAX/2]);
void resetujGre(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2], int* wynik);
void aktualizuj(waz* gracz, int wynik, int czasOdkad, int czasDokad, wektor* pozycjaPunktu, int rekord); /*nie wyswietla weza*/
void pobierzPodane(int argc, char* argv[], int* rozY, int* rozX, int* trudnosc);
int zczytajRekord();
void zapiszRekord(int wynik);

#endif
