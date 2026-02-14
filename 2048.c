/* Programarea Calculatoarelor, seria CC
 * Tema2 - 2048
 */
#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FOREVER 1
#define INIT_ROW 20
#define INIT_COL 20
#define BASE 1
#define HIGHLIGHT 2
#define LITTLE_BOX 3
#define OUTER_BOX 4
#define COLOR_ONE 5
#define COLOR_TWO 6
#define COL_THREE 7
#define COL_FOUR 8
#define COL_FIVE 9
#define COL_SIX 10
#define COL_SEVEN 11
#define COL_EIGHT 12
#define COL_NINE 13
#define COL_TEN 14
#define COL_ELEVEN 15
#define COLOR_GREEN2 16
#define COLOR_VIOLET 17
#define COLOR_GREEN3 18
#define COLOR_PINK 19
#define COLOR_PURPLE 20
#define COLOR_PURPLE2 21
#define COLOR_ORANGE 22
#define LOCKED 23
#define COLOR_GRAY 24
#define HELP_BOX 26

// structura care indica daca exista un joc inceput
typedef struct {
    bool gameStarted;
} gameState;

// structura care retine primii cei mai buni playeri
typedef struct {
    char name[50];
    int score;
} player;

/* a.functia care deseneaza tabla de joc si afiseaza timpul, scorul si cel mai
 bun scor */
void drawBoard(int ncols, int nrows, int score, int highscore) {
    score = 0;
    highscore = 0;

    // Setare culori - forma variabile - obiect, text, fundal
    init_pair(LITTLE_BOX, COLOR_BLUE, COLOR_WHITE);
    init_pair(OUTER_BOX, COLOR_BLUE, COLOR_CYAN);

    // dimensiuni cutii (cadranul si celulele matricei + spatii intre celule)
    int cellWidth = 9;
    int outerBoxW = 37;
    int outerBoxH = 18;
    int innerBoxW = 8;
    int innerBoxH = 4;
    int gapX = 1;
    int gapY = 0;

    /* calculeaza pozitiile de start pentru cadran (le centreaza pe ecranul
     terminalului)*/
    int startY = (nrows - outerBoxH) / 2 + 2;
    int startX = (ncols - outerBoxW) / 2;

    time_t current_time;
    struct tm *timeinfo;
    char time_string[80];

    // preia timpul curent din sistem
    time(&current_time);
    timeinfo = localtime(&current_time);
    // il formateaza dupa urmatoarea structura
    strftime(time_string, sizeof(time_string), "%d.%m.%Y %H:%M:%S", timeinfo);

    // le aseaza astfel incat sa inceapa din colt stanga sus
    mvprintw(startY - 1, (ncols - (cellWidth * 4 + 1)) / 2, "%s", time_string);
    mvprintw(startY - 3, (ncols - (cellWidth * 4 + 1)) / 2, "Score: %d", score);
    mvprintw(startY - 2, (ncols - (cellWidth * 4 + 1)) / 2, "Highscore: %d",
             highscore);
    refresh();

    // desenam cadranul
    WINDOW *outerBox = newwin(outerBoxH, outerBoxW, startY, startX);
    wbkgd(outerBox, COLOR_PAIR(OUTER_BOX));
    box(outerBox, 0, 0);
    wrefresh(outerBox);

    // calculeaza pozitia de start a celulelor
    int innerStartY = startY + gapY + 1;
    int innerStartX = startX + gapX;

    // deseneaza celulele
    int i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            int y = innerStartY + i * (innerBoxH + gapY);
            int x = innerStartX + j * (innerBoxW + gapX);

            // deseneaza celulele
            WINDOW *innerBox = newwin(innerBoxH, innerBoxW, y, x);
            wbkgd(innerBox, COLOR_PAIR(LITTLE_BOX));
            box(innerBox, 0, 0);
            wrefresh(innerBox);
        }
    }

    // afiseaza un mesaj pentru a ajunge la instructiuni
    mvprintw(startY + 18, (ncols - (cellWidth * 4 + 1)) / 2,
             "Press H for help");
    refresh();
}

// b. functia care creeaza o copie a tablei de joc
void copy(int BOARD[][4], int cpBOARD[][4]) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            cpBOARD[i][j] = BOARD[i][j];
        }
    }
}

// functia care verifica ca doua table de joc sunt identice
int isSameBoard(int BOARD[][4], int cpBOARD[][4]) {
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (cpBOARD[i][j] != BOARD[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

// d.functia care verifica ca un joc s-a terminat
int endGame(int BOARD[][4], int cpBOARD[][4]) {
    int sameA, sameD, sameW, sameS;
    int i, j, k, l;

    copy(BOARD, cpBOARD);

    sameA = sameD = sameW = sameS = 0;

    // verificam daca putem sa mutam patratelele spre stanga
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            // verificam daca doua tablite alaturate sunt egale si le contopim
            if (cpBOARD[i][j] != 0) {
                for (k = j + 1; k < 4; k++) {
                    if (cpBOARD[i][k] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[i][k]) {
                            cpBOARD[i][j] += cpBOARD[i][k];
                            cpBOARD[i][k] = 0;
                        }
                    }
                    break;
                }
            }
        }

        // mutam fiecare valoare nenula in ordine
        l = 0;
        for (j = 0; j < 4; j++) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[i][l++] = cpBOARD[i][j];

                // se pun zerourile spre dreapta pentru a simula o mutare spre
                // stanga a tuturor elementelor nenule
                if (j != l - 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // daca tablele sunt identice, inseamna ca tasta A este invalida
    if (isSameBoard(BOARD, cpBOARD)) {
        sameA = 1;
    }

    // verificam daca putem muta tablitele spre dreapta
    copy(BOARD, cpBOARD);

    for (i = 0; i < 4; i++) {
        // verificam daca doua tablite alaturate sunt egale si le contopim
        for (j = 3; j > 0; j--) {
            if (cpBOARD[i][j] != 0) {
                for (k = j - 1; k >= 0; k--) {
                    if (cpBOARD[i][k] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[i][k]) {
                            cpBOARD[i][j] += cpBOARD[i][k];
                            cpBOARD[i][k] = 0;
                        }
                        break;
                    }
                }
            }
        }

        // permutam spre dreapta toate valorile nenule
        l = 3;
        for (j = 3; j >= 0; j--) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[i][l--] = cpBOARD[i][j];
                // se pun zerourile la stanga pentru a simula o mutare spre
                // dreapta a tuturor elementelor nenule
                if (j != l + 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // daca tablita ramane la fel, tasta D este invalida
    if (isSameBoard(BOARD, cpBOARD)) {
        sameD = 1;
    }

    // verificam daca putem muta celulele in sus
    copy(BOARD, cpBOARD);

    for (j = 0; j < 4; j++) {
        // verifica daca doua celule alaturate sunt egale (pe coloana) si le
        // contopeste
        for (i = 0; i < 3; i++) {
            if (cpBOARD[i][j] != 0) {
                for (k = i + 1; k < 4; k++) {
                    if (cpBOARD[k][j] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[k][j]) {
                            cpBOARD[i][j] += cpBOARD[k][j];
                            cpBOARD[k][j] = 0;
                        }
                        break;
                    }
                }
            }
        }

        // preia valorile nenule si le permuta in sus (spre stanga pentru un
        // tablou unidimensional)
        l = 0;
        for (i = 0; i < 4; i++) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[l++][j] = cpBOARD[i][j];

                // se pun zerourile in jos pentru a simula o mutare in sus a
                // tuturor elementelor nenule
                if (i != l - 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // daca tabla nu se schimba, inseamna ca tasta W este invalida
    if (isSameBoard(BOARD, cpBOARD)) {
        sameW = 1;
    }

    // verificam daca putem permuta in jos celulele
    copy(BOARD, cpBOARD);

    for (j = 0; j < 4; j++) {
        // preluam valorile nenule alaturate si le contopim
        for (i = 3; i > 0; i--) {
            if (cpBOARD[i][j] != 0) {
                for (k = i - 1; k >= 0; k--) {
                    if (cpBOARD[k][j] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[k][j]) {
                            cpBOARD[i][j] += cpBOARD[k][j];
                            cpBOARD[k][j] = 0;
                        }
                        break;
                    }
                }
            }
        }

        // permutam in jos celulele (spre dreapta pentru un tablou
        // unidimensional)
        l = 3;
        for (i = 3; i >= 0; i--) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[l--][j] = cpBOARD[i][j];

                // se pun zerourile in sus pentru a simula o mutare in jos a
                // tuturor elementelor nenule
                if (i != l + 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // daca tabla ramane la fel dupa mutari, inseamna ca tasta S e invalida
    if (isSameBoard(BOARD, cpBOARD)) {
        sameS = 1;
    }

    // daca toate tastele sunt invalide, inseamna ca jocul s-a terminat
    if (sameA == 1 && sameD == 1 && sameW == 1 && sameS == 1) {
        return 1;
    }

    return 0;
}

// e.functia care verifica daca un joc a fost castigat
int winGame(int BOARD[][4]) {
    int i, j;

    // daca apare pe tabla valoarea 2048, jocul a fost castigat
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (BOARD[i][j] == 2048) {
                return 1;
            }
        }
    }

    return 0;
}

// f.functia care calculeaza cea mai buna miscare (cea care elibereaza cele mai
// multe spatii)
char bestMove(int BOARD[][4], int cpBOARD[][4]) {
    int i, j, k, l, cntA, cntD, cntW, cntS;

    copy(BOARD, cpBOARD);

    // se muta spre stanga toate piesele nenule (la fel ca in functia endGame)
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (cpBOARD[i][j] != 0) {
                for (k = j + 1; k < 4; k++) {
                    if (cpBOARD[i][k] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[i][k]) {
                            cpBOARD[i][j] += cpBOARD[i][k];
                            cpBOARD[i][k] = 0;
                        }
                    }
                    break;
                }
            }
        }

        l = 0;
        for (j = 0; j < 4; j++) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[i][l++] = cpBOARD[i][j];

                if (j != l - 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // numaram spatiile goale dupa mutarea corespunzatoare tastei A daca tabla
    // difera in urma permutarii
    cntA = 0;
    for (i = 0; i < 4; i++) {
        if (isSameBoard(BOARD, cpBOARD)) {
            cntA = 0;
            break;
        }
        for (j = 0; j < 4; j++)
            if (cpBOARD[i][j] == 0)
                cntA++;
    }

    // se muta spre dreapta toate piesele nenule (la fel ca in endGame)
    copy(BOARD, cpBOARD);
    for (i = 0; i < 4; i++) {
        for (j = 3; j > 0; j--) {
            if (cpBOARD[i][j] != 0) {
                for (k = j - 1; k >= 0; k--) {
                    if (cpBOARD[i][k] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[i][k]) {
                            cpBOARD[i][j] += cpBOARD[i][k];
                            cpBOARD[i][k] = 0;
                        }
                        break;
                    }
                }
            }
        }

        l = 3;
        for (j = 3; j >= 0; j--) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[i][l--] = cpBOARD[i][j];
                if (j != l + 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // numara spatiile goale dupa permutara tablitelor spre dreapta daca tabla
    // difera
    cntD = 0;
    for (i = 0; i < 4; i++) {
        if (isSameBoard(BOARD, cpBOARD)) {
            cntD = 0;
            break;
        }

        for (j = 0; j < 4; j++)
            if (cpBOARD[i][j] == 0)
                cntD++;
    }

    // se muta in sus celulele nenule si se contopesc
    copy(BOARD, cpBOARD);

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 3; i++) {
            if (cpBOARD[i][j] != 0) {
                for (k = i + 1; k < 4; k++) {
                    if (cpBOARD[k][j] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[k][j]) {
                            cpBOARD[i][j] += cpBOARD[k][j];
                            cpBOARD[k][j] = 0;
                        }
                        break;
                    }
                }
            }
        }

        l = 0;
        for (i = 0; i < 4; i++) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[l++][j] = cpBOARD[i][j];
                if (i != l - 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // numaram spatiile goale daca tabla e diferita de cum era initial
    cntW = 0;
    for (i = 0; i < 4; i++) {
        if (isSameBoard(BOARD, cpBOARD)) {
            cntW = 0;
            break;
        }

        for (j = 0; j < 4; j++)
            if (cpBOARD[i][j] == 0)
                cntW++;
    }

    // se muta in jos elementele nenule si contopim
    copy(BOARD, cpBOARD);

    for (j = 0; j < 4; j++) {
        for (i = 3; i > 0; i--) {
            if (cpBOARD[i][j] != 0) {
                for (k = i - 1; k >= 0; k--) {
                    if (cpBOARD[k][j] != 0) {
                        if (cpBOARD[i][j] == cpBOARD[k][j]) {
                            cpBOARD[i][j] += cpBOARD[k][j];
                            cpBOARD[k][j] = 0;
                        }
                        break;
                    }
                }
            }
        }

        l = 3;
        for (i = 3; i >= 0; i--) {
            if (cpBOARD[i][j] != 0) {
                cpBOARD[l--][j] = cpBOARD[i][j];
                if (i != l + 1) {
                    cpBOARD[i][j] = 0;
                }
            }
        }
    }

    // numaram spatiile goale daca tabla difera in urma permutarii
    // corespunzatoare tastei S
    cntS = 0;
    for (i = 0; i < 4; i++) {
        if (isSameBoard(BOARD, cpBOARD)) {
            cntS = 0;
            break;
        }

        for (j = 0; j < 4; j++)
            if (cpBOARD[i][j] == 0)
                cntS++;
    }

    // calculam maximul de spatii goale si returnam tasta careia ii corespunde
    if (cntA >= cntD && cntA >= cntW && cntA >= cntS)
        return 'a';
    if (cntD >= cntA && cntD >= cntW && cntD >= cntS)
        return 'd';
    if (cntW >= cntA && cntW >= cntD && cntW >= cntS)
        return 'w';

    return 's';
}

// functia care afiseaza celulele jocului
void showBoard(int score, int highscore, int ncols, int nrows, int BOARD[][4]) {
    int i, j;
    int cellWidth = 9;
    int outerBoxW = 37;
    int outerBoxH = 18;
    int innerBoxW = 8;
    int innerBoxH = 4;
    int gapX = 1;
    int gapY = 0;
    int startY = (nrows - outerBoxH) / 2 + 2;
    int startX = (ncols - outerBoxW) / 2;
    int innerStartY = startY + gapY + 1;
    int innerStartX = startX + gapX;
    time_t current_time;
    struct tm *timeinfo;
    char time_string[80];

    // afiseaza timpul curent scorul si cel mai bun scor
    time(&current_time);
    timeinfo = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%d.%m.%Y %H:%M:%S", timeinfo);
    mvprintw(startY - 1, (ncols - (cellWidth * 4 + 1)) / 2, "%s", time_string);
    mvprintw(startY - 3, (ncols - (cellWidth * 4 + 1)) / 2, "Score: %d", score);
    mvprintw(startY - 2, (ncols - (cellWidth * 4 + 1)) / 2, "Highscore: %d",
             highscore);
    refresh();

    // definirea culorilor
    init_pair(LITTLE_BOX, COLOR_BLUE, COLOR_WHITE);
    init_color(COLOR_GREEN2, 174, 687, 567);
    init_color(COLOR_VIOLET, 723, 312, 757);
    init_color(COLOR_GREEN3, 377, 736, 121);
    init_color(COLOR_PINK, 875, 367, 689);
    init_color(COLOR_PURPLE, 466, 166, 874);
    init_color(COLOR_PURPLE2, 466, 97, 552);
    init_color(COLOR_ORANGE, 900, 700, 0);
    init_pair(COLOR_ONE, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_TWO, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COL_THREE, COLOR_BLACK, COLOR_VIOLET);
    init_pair(COL_FOUR, COLOR_YELLOW, COLOR_GREEN2);
    init_pair(COL_FIVE, COLOR_WHITE, COLOR_PURPLE);
    init_pair(COL_SIX, COLOR_YELLOW, COLOR_GREEN3);
    init_pair(COL_SEVEN, COLOR_WHITE, COLOR_PURPLE2);
    init_pair(COL_EIGHT, COLOR_BLACK, COLOR_ORANGE);
    init_pair(COL_NINE, COLOR_PURPLE2, COLOR_PINK);
    init_pair(COL_TEN, COLOR_GREEN2, COLOR_YELLOW);
    init_pair(COL_ELEVEN, COLOR_BLACK, COLOR_RED);

    // afiseaza celulele
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            // calculeaza coordonatele de inceput
            int y = innerStartY + i * (innerBoxH + gapY);
            int x = innerStartX + j * (innerBoxW + gapX);
            WINDOW *innerBox = newwin(innerBoxH, innerBoxW, y, x);

            // seteaza culoarea in functie de valoarea din celula
            if (BOARD[i][j] == 0) {
                wbkgd(innerBox, COLOR_PAIR(LITTLE_BOX));
                box(innerBox, 0, 0);
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 2) {
                wbkgd(innerBox, COLOR_PAIR(COLOR_ONE));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "2  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 4) {
                wbkgd(innerBox, COLOR_PAIR(COLOR_TWO));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "4  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 8) {
                wbkgd(innerBox, COLOR_PAIR(COL_THREE));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "8  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 16) {
                wbkgd(innerBox, COLOR_PAIR(COL_FOUR));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "16  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 32) {
                wbkgd(innerBox, COLOR_PAIR(COL_FIVE));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "32  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 64) {
                wbkgd(innerBox, COLOR_PAIR(COL_SIX));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "64  ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 128) {
                wbkgd(innerBox, COLOR_PAIR(COL_SEVEN));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "128 ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 256) {
                wbkgd(innerBox, COLOR_PAIR(COL_EIGHT));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "256 ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 512) {
                wbkgd(innerBox, COLOR_PAIR(COL_NINE));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "512 ");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 1024) {
                wbkgd(innerBox, COLOR_PAIR(COL_TEN));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "1024");
                wrefresh(innerBox);
            } else if (BOARD[i][j] == 2048) {
                wbkgd(innerBox, COLOR_PAIR(COL_ELEVEN));
                box(innerBox, 0, 0);
                mvwprintw(innerBox, innerBoxH / 2, (innerBoxW - 1) / 2, "2048");
                wrefresh(innerBox);
                break;
            }
        }
    }
}

// i. sorteaza scorurile dintr-un fisier
void sortScores(int numberPlayer) {
    int i, j, num;
    player copyPlayer, leaderboard[10];

    // citeste in vectorul leaderboard, valorile din fisier
    FILE *file = fopen("highscores.txt", "rb");

    // citim in gol pentru a scapa de valoarea care retine numarul de jucatori
    // (inca nemodificat in fisier)
    fscanf(file, "%d", &num);

    // citeste jucatorii
    for (i = 0; i < numberPlayer; i++) {
        fscanf(file, "%s %d", leaderboard[i].name, &leaderboard[i].score);
    }

    fclose(file);

    // sorteaza jucatorii dupa scor
    for (i = 0; i < numberPlayer - 1; i++) {
        for (j = i + 1; j < numberPlayer; j++) {
            if (leaderboard[i].score < leaderboard[j].score) {
                strcpy(copyPlayer.name, leaderboard[i].name);
                strcpy(leaderboard[i].name, leaderboard[j].name);
                strcpy(leaderboard[j].name, copyPlayer.name);

                copyPlayer.score = leaderboard[i].score;
                leaderboard[i].score = leaderboard[j].score;
                leaderboard[j].score = copyPlayer.score;
            }
        }
    }

    // rescrie fisierul cu numarul nou de jucatori si jucatorii sortati
    file = fopen("highscores.txt", "w");

    fprintf(file, "%d\n", numberPlayer);

    for (i = 0; i < numberPlayer && i < 7; i++) {
        fprintf(file, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
    }

    fclose(file);
}

// j. functia care retine primele 5 scoruri cele mai mari
void saveHighscores(player leaderboard[], int *numberPlayer, int nrows,
                    int ncols, int *score) {
    clear();

    // centreaza textul
    int startY = nrows / 2;
    int startX = (ncols - strlen("Enter your name: ")) / 2;

    mvaddstr(startY, startX, "Enter your name: ");
    refresh();

    char namePlayer[500];
    char ch;
    int position = 0;

    // citeste un nume de la tastatura (nu sunt permise greseli sau alte
    // caractere care nu sunt litere sau numere)
    while ((ch = getch()) != '\n' && position < sizeof(namePlayer) - 1) {
        mvaddch(startY, startX + strlen("Enter your name: ") + position, ch);
        refresh();

        if (isalpha(ch) || isdigit(ch)) {
            namePlayer[position] = ch;
            position++;
        }
    }

    namePlayer[position] = '\0';

    // salveaza noile date in vectorul leaderboard
    strncpy(leaderboard[*numberPlayer].name, namePlayer,
            sizeof(leaderboard[*numberPlayer].name) - 1);
    leaderboard[*numberPlayer]
        .name[sizeof(leaderboard[*numberPlayer].name) - 1] = '\0';
    leaderboard[*numberPlayer].score = *score;

    // adauga jucatorul la finalul fisierului
    FILE *file = fopen("highscores.txt", "a");

    if (file != NULL) {
        fwrite(namePlayer, 1, strlen(namePlayer), file);
        fputc(' ', file);
        fprintf(file, " %d\n", *score);
        fclose(file);
    }

    if ((*numberPlayer) < 7)
        (*numberPlayer)++;

    // sorteaza fisier
    sortScores(*numberPlayer);
}

// afiseaza un top 5 al jucatorilor cu scorul cel mai mare
void showLeaderboard(player leaderboard[], int numberPlayer, int nrows,
                     int ncols) {
    clear();

    int i;
    int startY = (nrows - 5 * 2) / 2;

    mvprintw(startY - 2, (ncols - strlen("Leaderboard")) / 2, "Leaderboard");
    mvprintw(startY - 1, (ncols - strlen("==============")) / 2,
             "==============");

    FILE *file = fopen("highscores.txt", "rb");

    if (file != NULL) {
        fscanf(file, "%d", &numberPlayer);
        for (i = 0; i < numberPlayer && i < 5; i++) {
            fscanf(file, "%s %d", leaderboard[i].name, &leaderboard[i].score);
            mvprintw(startY, (ncols - strlen("==============")) / 2, "%d. %s",
                     i + 1, leaderboard[i].name);
            mvprintw(startY + 1, (ncols - strlen("Score: ")) / 2, "Score: %d",
                     leaderboard[i].score);
            startY += 2;
        }
    }

    mvaddstr(startY + 2,
             (ncols - strlen("Press ANY key to go back to the menu")) / 2,
             "Press ANY key to go back to the menu");
    refresh();

    fclose(file);
}

// functia in care se desfasoara jocul
void newGame(gameState *state, int *score, int *highscore, int BOARD[][4],
             int cpBOARD[][4], int ncols, int nrows, int *numberPlayer) {
    int ranNum, xRan, yRan, i, j, k, l;

    // deseneaza tabla
    clear();
    drawBoard(ncols, nrows, *score, *highscore);
    showBoard(*score, *highscore, ncols, nrows, BOARD);

    if (state->gameStarted == 0) {
        *score = 0;
        // initializam celulele tablei cu 0
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                BOARD[i][j] = 0;
            }
        }

        // obtinem pozitia randomizata pentru primul numar
        xRan = rand() % 4;
        yRan = rand() % 4;
        // obtinem un numar dintre 2 si 4
        ranNum = (rand() % 2) ? 2 : 4;

        // pozitionam primul numar
        BOARD[xRan][yRan] = ranNum;

        // afisam primul numar
        showBoard(*score, *highscore, ncols, nrows, BOARD);
        refresh();

        while (1) {
            // obtinem pozitia randomizata pentru al doilea numar
            xRan = rand() % 4;
            yRan = rand() % 4;
            // obtinem un numar dintre 2 si 4
            ranNum = (rand() % 2) ? 2 : 4;
            if (BOARD[xRan][yRan] == 0)
                break;
        }

        // pozitionam al doilea numar
        BOARD[xRan][yRan] = ranNum;

        // afisam al doilea numar
        showBoard(*score, *highscore, ncols, nrows, BOARD);
        refresh();
    }

    // jocul a inceput
    state->gameStarted = 1;

    int lose = 0;
    char key2 = ' ';
    int inactivity = 0;

    // cat timp jocul nu e pierdut sau castigat, jocul ruleaza
    while (!lose) {
        nodelay(stdscr, TRUE);
        timeout(100);

        // se citeste o cheie
        char key = getch();
        cbreak();

        // daca o tasta e apasata, miscarea automata e resetata
        if (key != ERR) {
            inactivity = 0;

            if (tolower(key2) == 'q') {
                break;
            }

            if (tolower(key) == 'q') {

                break;
            }
        } else {
            // daca nicio tasta nu e apasata se aduna cate o milisecunda
            inactivity += 100;

            /* daca nu s-a apasat nicio tasta timp de un minut, se calculeaza
             * cea mai buna miscare si executam miscarea care elibereaza cele
             * mai multe spatii */
            if (inactivity >= 60000) {
                key = bestMove(BOARD, cpBOARD);

                // resetam
                inactivity = 0;

                // afisam celulele
                showBoard(*score, *highscore, ncols, nrows, BOARD);
                refresh();
            }
        }

        copy(BOARD, cpBOARD);

        // produce schimbarile in functie de tastele apasate
        switch (tolower(key)) {
        case 'a':
            // mutam spre stanga valorile nenule si le contopim
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 3; j++) {
                    if (BOARD[i][j] != 0) {
                        for (k = j + 1; k < 4; k++) {
                            if (BOARD[i][k] != 0) {
                                if (BOARD[i][j] == BOARD[i][k]) {
                                    BOARD[i][j] += BOARD[i][k];
                                    BOARD[i][k] = 0;

                                    *score += BOARD[i][j];
                                    if (*score > *highscore) {
                                        *highscore = *score;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                l = 0;
                for (j = 0; j < 4; j++) {
                    if (BOARD[i][j] != 0) {
                        BOARD[i][l++] = BOARD[i][j];
                        if (j != l - 1) {
                            BOARD[i][j] = 0;
                        }
                    }
                }
            }

            // obtinem pozitia randomizata
            if (!isSameBoard(BOARD, cpBOARD)) {
                int emptySpace = 0;
                while (!emptySpace) {
                    xRan = rand() % 4;
                    yRan = rand() % 4;
                    // obtinem un numar dintre 2 si 4
                    ranNum = (rand() % 2) ? 2 : 4;

                    // pozitionam
                    if (BOARD[xRan][yRan] == 0) {
                        BOARD[xRan][yRan] = ranNum;
                        emptySpace = 1;
                    }
                }

                showBoard(*score, *highscore, ncols, nrows, BOARD);
                refresh();
            }

            break;

        case 'd':
            // mutam spre dreapta valorile nenule si le contopim
            for (i = 0; i < 4; i++) {
                for (j = 3; j > 0; j--) {
                    if (BOARD[i][j] != 0) {
                        for (k = j - 1; k >= 0; k--) {
                            if (BOARD[i][k] != 0) {
                                if (BOARD[i][j] == BOARD[i][k]) {
                                    BOARD[i][j] += BOARD[i][k];
                                    BOARD[i][k] = 0;

                                    *score += BOARD[i][j];
                                    if (*score > *highscore) {
                                        *highscore = *score;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                l = 3;
                for (j = 3; j >= 0; j--) {
                    if (BOARD[i][j] != 0) {
                        BOARD[i][l--] = BOARD[i][j];
                        if (j != l + 1) {
                            BOARD[i][j] = 0;
                        }
                    }
                }
            }

            // obtinem pozitia randomizata
            if (!isSameBoard(BOARD, cpBOARD)) {
                int emptySpace = 0;
                while (!emptySpace) {

                    xRan = rand() % 4;
                    yRan = rand() % 4;
                    // obtinem un numar dintre 2 si 4
                    ranNum = (rand() % 2) ? 2 : 4;

                    // pozitionam
                    if (BOARD[xRan][yRan] == 0) {
                        BOARD[xRan][yRan] = ranNum;
                        emptySpace = 1;
                    }
                }
                showBoard(*score, *highscore, ncols, nrows, BOARD);
                refresh();
            }

            break;

        case 'w':
            // mutam in sus valorile nenule si le contopim
            for (j = 0; j < 4; j++) {
                for (i = 0; i < 3; i++) {
                    if (BOARD[i][j] != 0) {
                        for (k = i + 1; k < 4; k++) {
                            if (BOARD[k][j] != 0) {
                                if (BOARD[i][j] == BOARD[k][j]) {
                                    BOARD[i][j] += BOARD[k][j];
                                    BOARD[k][j] = 0;

                                    *score += BOARD[i][j];
                                    if (*score > *highscore) {
                                        *highscore = *score;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                l = 0;
                for (i = 0; i < 4; i++) {
                    if (BOARD[i][j] != 0) {
                        BOARD[l++][j] = BOARD[i][j];
                        if (i != l - 1) {
                            BOARD[i][j] = 0;
                        }
                    }
                }
            }

            // obtinem pozitia randomizata
            if (!isSameBoard(BOARD, cpBOARD)) {
                int emptySpace = 0;
                while (!emptySpace) {
                    ;
                    xRan = rand() % 4;
                    yRan = rand() % 4;
                    // obtinem un numar dintre 2 si 4
                    ranNum = (rand() % 2) ? 2 : 4;

                    // pozitionam
                    if (BOARD[xRan][yRan] == 0) {
                        BOARD[xRan][yRan] = ranNum;
                        emptySpace = 1;
                    }
                }

                showBoard(*score, *highscore, ncols, nrows, BOARD);
                refresh();
            }

            break;

        case 's':
            // mutam in jos valorile nenule si le contopim
            for (j = 0; j < 4; j++) {
                for (i = 3; i > 0; i--) {
                    if (BOARD[i][j] != 0) {
                        for (k = i - 1; k >= 0; k--) {
                            if (BOARD[k][j] != 0) {
                                if (BOARD[i][j] == BOARD[k][j]) {
                                    BOARD[i][j] += BOARD[k][j];
                                    BOARD[k][j] = 0;

                                    *score += BOARD[i][j];
                                    if (*score > *highscore) {
                                        *highscore = *score;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                l = 3;
                for (i = 3; i >= 0; i--) {
                    if (BOARD[i][j] != 0) {
                        BOARD[l--][j] = BOARD[i][j];
                        if (i != l + 1) {
                            BOARD[i][j] = 0;
                        }
                    }
                }
            }

            // obtinem pozitia randomizata
            if (!isSameBoard(BOARD, cpBOARD)) {
                int emptySpace = 0;
                while (!emptySpace) {
                    xRan = rand() % 4;
                    yRan = rand() % 4;
                    // obtinem un numar dintre 2 si 4
                    ranNum = (rand() % 2) ? 2 : 4;

                    // pozitionam
                    if (BOARD[xRan][yRan] == 0) {
                        BOARD[xRan][yRan] = ranNum;
                        emptySpace = 1;
                    }
                }

                // afiseaza tabla
                showBoard(*score, *highscore, ncols, nrows, BOARD);
                refresh();
            }

            break;
        case 'h':
            clear();
            start_color();
            init_pair(HELP_BOX, COLOR_BLUE, COLOR_WHITE);

            // centrare text
            int outerBoxW = 37;
            int outerBoxH = 18;
            int startY = (nrows - outerBoxH) / 2 + 2;
            int startX = (ncols - outerBoxW) / 2;

            WINDOW *boxH = newwin(outerBoxH, outerBoxW, startY, startX);
            wbkgd(boxH, COLOR_PAIR(HELP_BOX));
            box(boxH, 0, 0);
            wrefresh(boxH);

            // lista comenzilor valide
            mvaddstr(startY + 1, startX + 2, "Commands");
            mvaddstr(startY + 2, startX + 3,
                     "You can move the tiles using the keys:");
            mvaddstr(startY + 3, startX + 3, "A - left");
            mvaddstr(startY + 4, startX + 3, "D - right");
            mvaddstr(startY + 5, startX + 3, "W - up");
            mvaddstr(startY + 6, startX + 3, "S - down");
            mvaddstr(startY + 7, startX + 2, "Press Q to return to the menu.");
            mvaddstr(startY + 8, startX + 2, "Press B to return to the game.");

            // se adauga timpul, scorul si cel mai bun scor
            time_t current_time;
            struct tm *timeinfo;
            char time_string[80];

            time(&current_time);
            timeinfo = localtime(&current_time);
            strftime(time_string, sizeof(time_string), "%d.%m.%Y %H:%M:%S",
                     timeinfo);
            mvprintw(startY + 10, startX + 2, "Date: %s", time_string);
            mvprintw(startY + 11, startX + 2, "Score: %d", *score);
            mvprintw(startY + 12, startX + 2, "Highscore: %d", *highscore);
            refresh();

            // se verifica daca sunt citite tastele q si b
            key2 = getch();
            cbreak();
            while (1) {
                if (tolower(key2) == 'q') {
                    break;
                } else if (tolower(key2) == 'b') {
                    clear();
                    drawBoard(ncols, nrows, *score, *highscore);
                    showBoard(*score, *highscore, ncols, nrows, BOARD);
                    refresh();
                    break;
                } else {
                    key2 = getch();
                    cbreak();
                }
            }
            break;
        }

        player leaderboard[10];

        // verificam daca jocul a fost castigat
        if (winGame(BOARD)) {
            // marcheaza sfarsitul jocului
            lose = 1;

            // resetarea jocului (nu se poate reveni la el)
            state->gameStarted = 0;
            clear();

            // afisare mesaj pentru cazul in care jocul e castigat
            mvaddstr(nrows / 2 - 6, (ncols - strlen("YOU WON :D")) / 2,
                     "YOU WON :D");
            mvaddstr(nrows / 2 - 4,
                     (ncols -
                      strlen("Congratulations on your amazing performance!")) /
                         2,
                     "Congratulations on your amazing performance!");
            mvaddstr(nrows / 2 - 3,
                     (ncols - strlen("I never finished the game. :3 ")) / 2,
                     "I never finished the game. :3 ");
            int scoreL = snprintf(NULL, 0, "Your score: %d", *score);
            mvprintw(nrows / 2 - 1, (ncols - scoreL) / 2, "Your score: %d",
                     *score);

            scoreL = snprintf(NULL, 0, "Highscore: %d", *highscore);
            mvprintw(nrows / 2, (ncols - scoreL) / 2, "Highscore: %d",
                     *highscore);

            mvaddstr(nrows / 2 + 2,
                     (ncols - strlen("Press ANY key to continue")) / 2,
                     "Press ANY key to continue");
            refresh();

            nodelay(stdscr, FALSE);
            getch();

            // afisare tabela de scor
            FILE *file = fopen("highscores.txt", "rb");
            fscanf(file, "%d", numberPlayer);
            saveHighscores(leaderboard, numberPlayer, nrows, ncols, score);
            showLeaderboard(leaderboard, *numberPlayer, nrows, ncols);
            fclose(file);

            nodelay(stdscr, FALSE);
            getch();
        }

        // verifica daca jocul s-a incheiat
        if (endGame(BOARD, cpBOARD)) {
            // marcheaza sfarsitul jocului
            lose = 1;
            state->gameStarted = 0;
            clear();

            // afiseaza mesaj in cazul in care jocul e pierdut
            mvaddstr(nrows / 2 - 4, (ncols - strlen("GAME OVER")) / 2,
                     "GAME OVER");
            int scoreL = snprintf(NULL, 0, "Your score: %d", *score);
            mvprintw(nrows / 2 - 2, (ncols - scoreL) / 2, "Your score: %d",
                     *score);
            scoreL = snprintf(NULL, 0, "Highscore: %d", *highscore);
            mvprintw(nrows / 2 - 1, (ncols - scoreL) / 2, "Highscore: %d",
                     *highscore);
            mvaddstr(nrows / 2 + 2,
                     (ncols - strlen("Press ANY key to continue")) / 2,
                     "Press ANY key to continue");
            refresh();

            getchar();

            // se afiseaza tabela de scor
            FILE *file = fopen("highscores.txt", "rb");
            fscanf(file, "%d", numberPlayer);
            saveHighscores(leaderboard, numberPlayer, nrows, ncols, score);
            showLeaderboard(leaderboard, *numberPlayer, nrows, ncols);
            fclose(file);

            getchar();
        }
    }
}

int main(void) {
    int nrows, ncols, numberPlayer = 0;
    int i, score = 0, highscore = 0;
    int BOARD[4][4], cpBOARD[4][4];
    gameState state;
    char x[20];

    // 1.Jocul nu a inceput
    state.gameStarted = 0;

    // Se inițializează ecranul
    WINDOW *wnd = initscr();

    // Se ascunde cursorul
    curs_set(0);

    // Permite functionalitatea culorilor
    start_color();

    // Setare culori - forma variabile - obiect, text, fundal
    init_color(COLOR_GRAY, 192, 192, 192);
    init_pair(BASE, COLOR_BLUE, COLOR_CYAN);
    init_pair(HIGHLIGHT, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(LOCKED, COLOR_BLACK, COLOR_GRAY);
    bkgd(COLOR_PAIR(BASE));

    // Desenare caseta
    box(wnd, 0, 0);

    // Obtine dimensiunile ferestrei
    getmaxyx(wnd, nrows, ncols);

    // Se inhibă afișarea caracterelor introduse de la tastatură
    noecho();

    // Caracterele introduse sunt citite imediat - fără 'buffering'
    cbreak();

    // Permite folosirea tastelor speciale
    keypad(wnd, 1);

    // Optiuni meniu
    char *choices[] = {"> New Game    ", "> Resume      ", "> Quit        "};

    // initializam highscore-ul
    FILE *file = fopen("highscores.txt", "r");
    fscanf(file, "%d", &i);
    fscanf(file, "%s", x);
    fscanf(file, "%d", &highscore);
    fclose(file);

    // 2.Ruleaza la infinit (bucla jocului care se opreste dupa selectarea
    // optiunii Quit)
    while (FOREVER) {
        int option;
        int selected = 0;

        // meniu
        while (1) {
            clear();

            // Lucruri strict legate de design
            mvaddstr((nrows - 7) / 2,
                     (ncols - strlen("-------2048-------")) / 2,
                     "-------2048-------");
            mvaddstr((nrows - 7) / 2 + 6,
                     (ncols - strlen("-----------------")) / 2,
                     "------------------");

            // Coloram meniul
            for (i = 0; i < 3; i++) {
                // verifica daca exista joc inceput pentru a colora
                // corespunzator optiunea Resume
                if (i == selected && state.gameStarted == 0 && i == 1) {
                    wattron(wnd, COLOR_PAIR(LOCKED));
                } else if (i == selected) {
                    wattron(wnd, COLOR_PAIR(HIGHLIGHT));
                }
                mvwprintw(wnd, (nrows - 7) / 2 + i + 2,
                          (ncols - strlen(choices[i])) / 2, "%s", choices[i]);
                wattroff(wnd, COLOR_PAIR(HIGHLIGHT));
            }
            wrefresh(wnd);

            // citim o tasta pentru a selecta optiunea
            option = wgetch(wnd);
            // in cazul KEY_UP si w muta optiunea selectata in sus, cazul
            // KEY_DOWN si s, in jos
            switch (tolower(option)) {
            case KEY_UP:
                selected--;
                if (selected == -1)
                    selected = 0;
                break;
            case 'w':
                selected--;
                if (selected == -1)
                    selected = 0;
                break;
            case KEY_DOWN:
                selected++;
                if (selected == 3)
                    selected = 2;
                break;
            case 's':
                selected++;
                if (selected == 3)
                    selected = 2;
                break;
            case 10:
                // optiunea New Game
                if (selected == 0) {
                    score = 0;
                    drawBoard(ncols, nrows, score, highscore);
                    state.gameStarted = 0;

                    while (1) {
                        newGame(&state, &score, &highscore, BOARD, cpBOARD,
                                ncols, nrows, &numberPlayer);
                        break;
                    }
                    break;
                } else if (selected == 1 && state.gameStarted == 1) {
                    // optiunea Resume se poate selecta doar daca exista joc
                    // inceput
                    while (1) {
                        newGame(&state, &score, &highscore, BOARD, cpBOARD,
                                ncols, nrows, &numberPlayer);
                        break;
                    }
                    break;
                } else if (selected == 2) {
                    // iese din program (ne intoarcem la terminal)
                    endwin();
                    return 0;
                }
                break;
            }
        }
    }

    // Se reflectă schimbările pe ecran
    refresh();

    endwin();

    return 0;
}
