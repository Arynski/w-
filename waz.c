#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <argp.h>

#include "struktury.h"

extern int PLANSZAX; /*40*/
extern int PLANSZAY; /*20*/
extern int TRUDNOSC; /*1 - trudny, 2 - sredni, 3 - latwy*/

int main(int argc, char *argv[]) {
  pobierzPodane(argc, argv, &PLANSZAY, &PLANSZAX, &TRUDNOSC);
  srand(time(NULL));
  int poczatek = 0; /*odliczanie czasu*/
  int wejscie = 0;
  bool bylDodany = false;
  bool wyjscie = false;
  int wynik = 0;
  int rekord;
  waz* gracz = stworzWeza('o', '@');
  bool plansza[PLANSZAY][PLANSZAX/2];
  wektor pozycjaPunktu; pozycjaPunktu.x = rand()%(PLANSZAX) + 2; pozycjaPunktu.y = rand()%PLANSZAY + 1;

  /*INICJOWANIE*/
  for(int i = 0; i < PLANSZAY; ++i)
    for(int j = 0; j < PLANSZAX/2; ++j)
      plansza[i][j] = false;
  setlocale(LC_ALL, "");
  initscr(); /*tworzy wirtualne okno, zajmuje tyle pamieci, ile potrzebuje na cale okno*/
  curs_set(0);
  keypad(stdscr, TRUE);
  /*KONIEC INICJOWANIA*/

  aktualizuj(gracz, wynik, 0, 0, &pozycjaPunktu, rekord);
  aktualizujWeza(gracz, plansza, wejscie, &bylDodany);
  
  while(!wyjscie) {
    rekord = zczytajRekord();
    wejscie = getch(); /*czekamy az zacznie grac*/
    poczatek = time(NULL); /*zacznij odliczac czas*/
    halfdelay(TRUDNOSC);

    /*GŁÓWNA PETLA GRY*/
    while(1) {
      clear();
      aktualizuj(gracz, wynik, poczatek, time(NULL), &pozycjaPunktu, rekord);
      if(!aktualizujWeza(gracz, plansza, wejscie, &bylDodany)) { /*wjechal w samego siebie*/
	wyswietlWeza(gracz);
	refresh();
	break;
      }
      
      if(gracz->cialo.tablica[0].pozycja.x == pozycjaPunktu.x/2 && gracz->cialo.tablica[0].pozycja.y == pozycjaPunktu.y) {
	dodajDoWeza(gracz, plansza);
	bylDodany = true;
	wynik++;
	pozycjaPunktu.x = rand()%(PLANSZAX/2) + 2; pozycjaPunktu.y = rand()%PLANSZAY + 1;
	while(plansza[pozycjaPunktu.y-1][pozycjaPunktu.x/2-1]) {
	  pozycjaPunktu.x = rand()%(PLANSZAX/2) + 2; pozycjaPunktu.y = rand()%PLANSZAY + 1;
	}
      }
      refresh();
      wejscie = getch();
    }
    /*GŁÓWNA PETLA GRY*/
    mvprintw(PLANSZAY+3, 0, "NACISNIJ DOWOLNY PRZYCISK!");
    if(wynik > rekord)
      zapiszRekord(wynik);
    resetujGre(gracz, plansza, &wynik);
    nocbreak();
  }
  
  endwin(); /*uwalnia pamiec, wylacza ncurses*/
  return 0;
}
