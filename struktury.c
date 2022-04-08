#include "struktury.h"
#include <ncurses.h>
#include <unistd.h> 

int PLANSZAX = 40; /*40*/
int PLANSZAY = 20; /*20*/
int TRUDNOSC = 1; //1 - trudny, 2 - sredni, 3 - latwy

void alokujDyntab(dyntab* tab) {
  tab->tablica = (czescWeza*) malloc(2 * sizeof(czescWeza));
  tab->zuzyte = 0;
  tab->rozmiar = 2;
}

void dodajDyntab(dyntab* tab, czescWeza* x) {
  if(tab->zuzyte == tab->rozmiar) {
    tab->rozmiar *= 2; /*dzieki temu najpierw jest zaalokowana na 2, potem 4, 8, 16 itd...*/
    tab->tablica = (czescWeza*) realloc(tab->tablica, tab->rozmiar * sizeof(czescWeza));
  }

  tab->tablica[tab->zuzyte++] = *x;
}

void zwolnijDyntab(dyntab* tab) {
  free(tab->tablica);
  tab->tablica = NULL;
  tab->zuzyte = tab->rozmiar = 0;
}


void wyswietlPlansze() {
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, PLANSZAX+3, ACS_URCORNER);
  mvaddch(PLANSZAY+1, 0, ACS_LLCORNER);
  mvaddch(PLANSZAY+1, PLANSZAX+3, ACS_LRCORNER);
  for(int i = 0; i < PLANSZAX+2; ++i)  {
    mvaddch(0, 1+i, ACS_HLINE);
    mvaddch(PLANSZAY+1, 1+i, ACS_HLINE);
  }
  for(int i = 0; i < PLANSZAY; ++i) {
    mvaddch(1+i, 0, ACS_VLINE);
    mvaddch(1+i, 1, ACS_VLINE);
    mvaddch(1+i, PLANSZAX+2, ACS_VLINE);
    mvaddch(1+i, PLANSZAX+3, ACS_VLINE);
  }
}

waz* stworzWeza(char cialo, char glowa) {
  waz* zwroc = (waz*) malloc(sizeof(waz));
  zwroc->cialoWeza = cialo;
  zwroc->glowaWeza = glowa;
  zwroc->dlugosc = 1;
  zwroc->kurs.x = zwroc->kurs.y = 0;

  alokujDyntab(&zwroc->cialo);
  dodajDyntab(&zwroc->cialo, stworzCzescWeza(PLANSZAY/2, PLANSZAX/4, 1));
  return zwroc;
}

czescWeza* stworzCzescWeza(int y, int x, bool _czyGlowa) {
  czescWeza* zwroc = (czescWeza*) malloc(sizeof(czescWeza));
  zwroc->czyGlowa = _czyGlowa;
  zwroc->pozycja.x = x;
  zwroc->pozycja.y = y;

  return zwroc;
}

void wyswietlWeza(waz* postac) {
  for(int i = 1; i < postac->dlugosc; ++i) {
    mvaddch(postac->cialo.tablica[i].pozycja.y, postac->cialo.tablica[i].pozycja.x*2, ACS_BLOCK);
    mvaddch(postac->cialo.tablica[i].pozycja.y, (postac->cialo.tablica[i].pozycja.x*2)+1, ACS_BLOCK);
  }

  /*glowe wyswietla na koncu zeby nachodzila*/
  mvaddch(postac->cialo.tablica[0].pozycja.y, postac->cialo.tablica[0].pozycja.x*2, ACS_CKBOARD);
  mvaddch(postac->cialo.tablica[0].pozycja.y, (postac->cialo.tablica[0].pozycja.x*2)+1, ACS_CKBOARD);
    
  
}

bool czyPorazka(waz* waz) {
  wektor pozycja = waz->cialo.tablica[0].pozycja;
  if(pozycja.y > PLANSZAY || pozycja.y <= 0
     || pozycja.x <= 0 || pozycja.x >= PLANSZAX/2+1)
    return true;

  /*czy nie wchodzi sam w siebie*/
  
  return false;
}

void ustawKierunek(waz* waz, int wejscie) {
  wektor poprzedniKurs = waz->kurs;
  switch(wejscie) {
  case LEWO:
    waz->kurs.x = -1;
    waz->kurs.y = 0;
    break;
  case PRAWO:
    waz->kurs.x = 1;
    waz->kurs.y = 0;
    break;
  case GORA:
    waz->kurs.x = 0;
    waz->kurs.y = -1;
    break;
  case DOL:
    waz->kurs.x = 0;
    waz->kurs.y = 1;
    break;
  default:
    break;
  }
  if(poprzedniKurs.x * waz->kurs.x == -1 || poprzedniKurs.y * waz->kurs.y == -1)
    waz->kurs = poprzedniKurs;
}

void zmienPozycje(int y, int x, czescWeza* cz) {
  cz->pozycja.x = x;
  cz->pozycja.y = y;
}

bool aktualizujWeza(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2], int kierunek, bool* bylDodany) {
  bool zwroc = true;
  czescWeza* glowa = &gracz->cialo.tablica[0];
  czescWeza* tablica = gracz->cialo.tablica;
  czescWeza* pamiec = (czescWeza*)malloc(2*sizeof(czescWeza*));
  ustawKierunek(gracz, kierunek);

  int pozycjaWezax = (glowa->pozycja.x + gracz->kurs.x)-1;
  int pozycjaWezay = (glowa->pozycja.y + gracz->kurs.y)-1;
  if(pozycjaWezax < 0 || pozycjaWezay < 0 || pozycjaWezay >= PLANSZAY || pozycjaWezax >= PLANSZAX/2) {
    mvprintw(pozycjaWezay+1, (pozycjaWezax+1)*2, "//");
    return false;
  }
  /*ostatna czesc weza idzie przed pierwsza zgodnie z kierunkiem*/
  if(*bylDodany)
    *bylDodany = false;

  plansza[gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.y-1][gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.x-1] = false;
  zwroc = !plansza[(glowa->pozycja.y + gracz->kurs.y)-1][(glowa->pozycja.x + gracz->kurs.x)-1];
  zmienPozycje(glowa->pozycja.y + gracz->kurs.y, glowa->pozycja.x + gracz->kurs.x, &tablica[gracz->cialo.zuzyte-1]);
  plansza[gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.y-1][gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.x-1] = true;
  
  /*ustawia ostatniego na pierwsza pozycje, a reszte cofa o jeden*/
  /*[4, 3, 2, 1, 0] -> [3, 2, 1, 0, 4]*/
  pamiec[0] = tablica[0];
  tablica[0] = tablica[gracz->cialo.zuzyte-1];
  for(int i = 1; i < gracz->cialo.zuzyte; ++i) {
    pamiec[i%2] = tablica[i];
    tablica[i] = pamiec[!(i%2)];
  }

  wyswietlWeza(gracz);
  refresh();

  return zwroc;
}

/*sekundy sa 00:xx, minuty xx:xx, godziny xx:xx:xx*/
void formatujCzas(int y, int x, int sekundy) {
  int godziny = sekundy / 3600;
  int minuty = (sekundy%3600) / 60;
  int sekund = sekundy%60;

  if(godziny == 0)
    mvprintw(y, x, "%02d:%02d", minuty, sekund);
  else
    mvprintw(y, x, "%02d:%02d:%02d", godziny, minuty, sekund);
}

void dodajDoWeza(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2]) {
  dodajDyntab(&gracz->cialo, stworzCzescWeza(gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.y, gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.x, false));
  gracz->dlugosc++;
  plansza[gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.y-1][gracz->cialo.tablica[gracz->cialo.zuzyte-1].pozycja.x-1] = true;
}

void wyswietlPlanszeBool(bool plansza[PLANSZAY][PLANSZAX/2]) {
  int x = PLANSZAX + 5;

  for(int i = 0; i < PLANSZAY; i++) {
    for(int j = 0; j < PLANSZAX/2; j++) {
      if(plansza[i][j])
	mvaddch(i, x+j, '*');
    }
  }
}

void resetujGre(waz* gracz, bool plansza[PLANSZAY][PLANSZAX/2], int* wynik) {
  *wynik = 0;
  
  for(int i = 0; i < PLANSZAY; ++i)
    for(int j = 0; j < PLANSZAX/2; ++j)
      plansza[i][j] = false;

  zwolnijDyntab(&gracz->cialo);
  free(gracz);
  gracz = stworzWeza('o', '@');
  gracz->cialo.tablica[0].pozycja.x = PLANSZAX/4;
  gracz->cialo.tablica[0].pozycja.y = PLANSZAY/2;
}

void aktualizuj(waz* gracz, int wynik, int czasOdkad, int czasDokad, wektor* pozycjaPunktu, int rekord) {
  mvprintw(PLANSZAY+4, 0, "Czas:");
  formatujCzas(PLANSZAY+4, 6, czasDokad - czasOdkad);
  mvprintw(PLANSZAY+5, 0, "Wynik: %d", wynik);
  mvprintw(pozycjaPunktu->y, pozycjaPunktu->x, "*");
  mvprintw(PLANSZAY+2, 0, "Pozycja (%d; %d)", gracz->cialo.tablica[0].pozycja.x, gracz->cialo.tablica[0].pozycja.y);
  mvprintw(PLANSZAY+6, 0, "Rekord: %d", rekord);
  wyswietlPlansze();
}

void pobierzPodane(int argc, char* argv[], int* rozY, int* rozX, int* trudnosc) {
  mvprintw(1, 1, "%c", argv[0][2]);

  /* -x liczba wymiary x planszy */
  /* -y liczba wymiary y planszy */
  /* -t [1/2/3] trudnosc*/
  int opt;
  while((opt = getopt(argc, argv, "x:y:t:")) != -1) {
    switch(opt) {
    case 'x':
      *rozX = atoi(optarg)*2;
      break;
    case 'y':
      *rozY = atoi(optarg);
      break;
    case 't':
      *trudnosc = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Nieznana flaga: %s\n", optarg);
    }
  }
}

int zczytajRekord() {
  FILE* plik = fopen("rekordy", "r");
  int xRekordu, yRekordu, tRekordu, wartosc;

  fscanf(plik, "%d %d %d", &yRekordu, &xRekordu, &tRekordu);
  while(!feof(plik)) {
    fscanf(plik, "%d", &wartosc);
    if(PLANSZAY == yRekordu && PLANSZAX == xRekordu*2 && TRUDNOSC == tRekordu) {
      fclose(plik);
      return wartosc;
    }

    fscanf(plik, "%d %d %d", &yRekordu, &xRekordu, &tRekordu);
  }
  
  fclose(plik);
  return 0;
}

void zapiszRekord(int wynik) {
  int xRekordu, yRekordu, tRekordu, wartosc;
  bool zapisane = false;
  if(rename("rekordy", "starerekordy"))
    return 1;
  FILE* stary = fopen("starerekordy", "r");
  FILE* nowy = fopen("rekordy", "w");
  FILE* logi = fopen("logi2", "w");
  
  fscanf(stary, "%d %d %d", &yRekordu, &xRekordu, &tRekordu);
  fprintf(logi, "Wartosci: %d %d %d", yRekordu, xRekordu, tRekordu);
  while(!feof(stary)) {
    fscanf(stary, "%d", &wartosc);
    fprintf(logi, "wynik wtedy: %d", wartosc);
    if(PLANSZAY == yRekordu && PLANSZAX == xRekordu*2 && TRUDNOSC == tRekordu) {
      fprintf(logi, "nadpisuje^!");
      fprintf(nowy, "%d %d %d\n%d\n", yRekordu, xRekordu, tRekordu, wynik);
      zapisane = true;
    }
    else {
      fprintf(logi, "zapisuje^!");
      fprintf(nowy, "%d %d %d\n%d\n", yRekordu, xRekordu, tRekordu, wartosc);
    }

    fscanf(stary, "%d %d %d", &yRekordu, &xRekordu, &tRekordu);
    fprintf(logi, "Wartosci: %d, %d, %d", yRekordu, xRekordu, tRekordu);
  }

  if(!zapisane) {
    fprintf(nowy, "%d %d %d\n%d\n", PLANSZAY, PLANSZAX/2, TRUDNOSC, wynik);
    fprintf(logi, "Nie znaleziono rekordu i zapisuje!");
  }

  fclose(stary); fclose(nowy); fclose(logi);  
}
/*                  
   _      ______ _ _//_
  | | /| / / __ `//_  /
  | |/ |/ / /_/ /  / /_
  |__/|__/\__, _\ /___/
              \\
*/

