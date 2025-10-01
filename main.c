#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUTFILE "beh.txt"
#define OUTPUTFILE "vysledkova_listina.txt"
#define MAX 100

typedef struct {
    int den;
    int mesic;
    int rok;
} DATUM;

typedef struct {
    int hod;
    int min;
    int sec;
} CAS;

typedef struct {
    int cislo;
    char prijmeni[15];
    char jmeno[15];
    DATUM datum;
    char stat[5];
    char klub[50];
    CAS cas;
} SOUTEZICI;

SOUTEZICI *readFile(SOUTEZICI *soutezici, int *pocet) {
    FILE *fr = fopen(INPUTFILE, "r");
    if (fr == NULL) {
        printf("Nepodarilo se otevrit soubor %s\n", INPUTFILE);
    }
    int i = 0;
    int j;
    char radek[MAX];
    char *token;
    SOUTEZICI *pomocny;
    while (fgets(radek, MAX, fr) != NULL) {
        if (i != 0) {
            pomocny = (SOUTEZICI *) realloc(soutezici, sizeof(SOUTEZICI) * i);
            if (pomocny == NULL) {
                printf("Neuspesna realokace pametoveho bloku.\n");
                free(soutezici);
            } else {
                soutezici = pomocny;
                token = strtok(radek, ";.:");
                j = 0;
                while (token != NULL) {
                    switch (j) {
                        case 0:
                            soutezici[i - 1].cislo = atoi(token);
                            break;
                        case 1:
                            strcpy(soutezici[i - 1].prijmeni, token);
                            break;
                        case 2:
                            strcpy(soutezici[i - 1].jmeno, token);
                            break;
                        case 3:
                            soutezici[i - 1].datum.den = atoi(token);
                            break;
                        case 4:
                            soutezici[i - 1].datum.mesic = atoi(token);
                            break;
                        case 5:
                            soutezici[i - 1].datum.rok = atoi(token);
                            break;
                        case 6:
                            strcpy(soutezici[i - 1].stat, token);
                            break;
                        case 7:
                            strcpy(soutezici[i - 1].klub, token);
                            break;
                        case 8:
                            soutezici[i - 1].cas.hod = atoi(token);
                            break;
                        case 9:
                            soutezici[i - 1].cas.min = atoi(token);
                            break;
                        case 10:
                            soutezici[i - 1].cas.sec = atoi(token);
                            break;
                    }
                    j++;
                    token = strtok(NULL, ";.:");
                }
            }
        }
        i++;
    }
    *pocet = i - 1;
    if (fclose(fr) == EOF) {
        printf("Nepodarilo se zavrit soubor %s.\n", INPUTFILE);
    }
    return soutezici;
}

int cas(int rocnik) {
    time_t t = time(NULL);
    struct tm *tim = localtime(&t);
    return (tim->tm_year + 1900) - rocnik;
}

void vypis(SOUTEZICI *soutezici, int pocet) {
    int cze = 0;
    int index = 0;
    int nejstarsi = soutezici[0].datum.rok;
    printf("S T A R T O V N I  L I S T I N A  -  M A R A T O N\n");
    printf("-----------------------------------------------------\n");
    printf("cislo|  prijmeni |    jmeno    |  narozen    |stat|           klub                |   cas  |\n");
    printf("--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < pocet; ++i) {
        printf("%3d| %12s| %12s| %02d.%02d.%d  | %s| %30s| %d:%02d:%02d|\n", soutezici[i].cislo, soutezici[i].prijmeni,
               soutezici[i].jmeno,
               soutezici[i].datum.den, soutezici[i].datum.mesic, soutezici[i].datum.rok, soutezici[i].stat,
               soutezici[i].klub,
               soutezici[i].cas.hod, soutezici[i].cas.min, soutezici[i].cas.sec);
        if (strcmp(soutezici[i].stat, "CZE") == 0) {
            cze++;
        }
        if (soutezici[i].datum.rok < nejstarsi) {
            nejstarsi = soutezici[i].datum.rok;
            index = i;
        }
    }
    printf("--------------------------------------------------------------------------------------------\n");
    printf("Celkovy pocet zavodniku je %d\n", pocet);
    printf("Cechu je %d\n", cze);
    printf("Nejstarsim zavodnikem je %s %s narozen v roce %d. Je mu %d let", soutezici[index].jmeno,
           soutezici[index].prijmeni, soutezici[index].datum.rok,
           cas(soutezici[index].datum.rok));
}

int prevod(CAS cas) {
    return cas.hod * 3600 + cas.min * 60 + cas.sec;
}

void swap(SOUTEZICI *temp1, SOUTEZICI *temp2) {
    SOUTEZICI xp = *temp1;
    *temp1 = *temp2;
    *temp2 = xp;
}

void bubblesort(SOUTEZICI *soutezici, int pocet) {
    for (int i = 0; i < pocet - 1; ++i) {
        for (int j = 0; j < pocet - 1 - i; ++j) {
            if (prevod(soutezici[j].cas) > prevod(soutezici[j + 1].cas)) {
                swap(&soutezici[j], &soutezici[j + 1]);
            }
        }
    }
}

void doSouboru(SOUTEZICI *soutezici, int pocet) {
    bubblesort(soutezici, pocet);
    FILE *fw = fopen(OUTPUTFILE, "w");
    if (fw == NULL) {
        printf("Nepodarilo se otevrit soubor %s.\n", OUTPUTFILE);
    } else {
        fprintf(fw, "S T A R T O V N I  L I S T I N A  -  M A R A T O N\n");
        fprintf(fw, "-----------------------------------------------------\n");
        fprintf(fw,
                "poradi|cislo|  prijmeni |    jmeno    |  narozen    |stat|           klub            |   cas  | ztrata |\n");
        fprintf(fw, "-------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < pocet; ++i) {
            if (i != 0){
            fprintf(fw, "%2d|%3d| %12s| %12s| %02d.%02d.%d  | %s| %30s| %d:%02d:%02d|+%d:%02d:%02d|\n", i + 1,
                    soutezici[i].cislo,
                    soutezici[i].prijmeni,
                    soutezici[i].jmeno,
                    soutezici[i].datum.den, soutezici[i].datum.mesic, soutezici[i].datum.rok, soutezici[i].stat,
                    soutezici[i].klub,
                    soutezici[i].cas.hod, soutezici[i].cas.min, soutezici[i].cas.sec,
                    (prevod(soutezici[i].cas) - prevod(soutezici[0].cas)) / 3600,
                    (prevod(soutezici[i].cas) - prevod(soutezici[0].cas)) % 3600 / 60,
                    (prevod(soutezici[i].cas) - prevod(soutezici[0].cas)) % 60);
            } else {
                fprintf(fw, "%2d|%3d| %12s| %12s| %02d.%02d.%d  | %s| %30s| %d:%02d:%02d|        |\n", i + 1,
                        soutezici[i].cislo,
                        soutezici[i].prijmeni,
                        soutezici[i].jmeno,
                        soutezici[i].datum.den, soutezici[i].datum.mesic, soutezici[i].datum.rok, soutezici[i].stat,
                        soutezici[i].klub,
                        soutezici[i].cas.hod, soutezici[i].cas.min, soutezici[i].cas.sec);
            }
        }
        fprintf(fw, "-------------------------------------------------------------------------------------------------------\n");
    }
    if (fclose(fw) == EOF){
        printf("Nepodarilo se zavrit soubor %s\n", OUTPUTFILE);
    }
}

int main() {
    SOUTEZICI *soutezici = NULL;
    int pocet = 0;
    soutezici = readFile(soutezici, &pocet);
    vypis(soutezici, pocet);
    doSouboru(soutezici, pocet);
    free(soutezici);
    return 0;
}
