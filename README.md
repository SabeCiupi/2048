# 2048
A classic 2048 game clone developed in C using the ncurses library for a terminal-based graphical user interface. This project was created as part of the Computer Programming series (CC).

## Prerequisites
To run this game, you need the ncurses library installed on your system.
- *Debian/Ubuntu:* `sudo apt-get install libncurses5-dev libncursesw5-dev`

## Build and Run
Maximize your terminal window before running the game to ensure the board renders correctly. Use the provided Makefile to compile and execute:
`make`

`make run`

`make clean`

## Gameplay & Controls
The goal is to slide numbered tiles on a grid to combine them to create a tile with the number 2048.

Controls
|Key|Action|
|W|Move Tiles Up|
|S |Move Tiles Down|
|A|Move Tiles Left |
|D |Move Tiles Right|
|H|Open Help/Commands Menu |
|Q|Quit to Menu|

## Special Features
- Resume System: If you exit to the menu without closing the app, you can pick up exactly where you left off.
- Automatic "Best Move": If the game detects 60 seconds of inactivity, it will automatically calculate and execute the move that clears the most space on the board.
- Dynamic Coloring: Every tile value has a unique color pair for better visibility.

## Leaderboard & Scoring
When you win or lose, you can save your legacy. The game maintains a highscores.txt file and displays the Top 5 players.
- Enter your name after a "Game Over" to save your score.
- The system automatically sorts scores so only the best appear on the leaderboard.

## Technical Overview
The program is organized into specialized sub-programs to handle game state and rendering:
Core Logic
- newGame / drawBoard: Handles the main game loop and initial grid rendering.
- endGame / winGame: Logic checks for available moves or the reaching of the 2048 tile.
- bestMove: An algorithm that simulates all four directions to find the move resulting in the most empty cells.
Data Management
- copy / isSameBoard: Utilities to track board changes and validate if a move is legal.
- saveHighscores / sortScores: Handles binary/text file I/O to maintain the persistent leaderboard.




### Varianta in romana (2023)

=================================== 2048 ===================================

Instructiuni (!!important!!):
Inainte de scrierea comenzii "make run", mariti terminalul. 

Prezentare variabile din program:
> nrows, ncols - numarul maxim de linii si coloane
> i, j, k, l, cpBOARD[4][4] - variabile de manevra
> score - retine scorul din jocul curent
> highscore - retine cel mai bun scor din sesiunea curenta 
> BOARD[4][4] - retine valorile si pozitiile corespunzatoare fiecarei celule 
> state - variabila de tip structura care retine daca exista un joc inceput

Prezentare program:
(1) Facem initializarile scorului, celui mai bun scor si a starii cu 0 pentru
a indica ca nu exista niciun joc inceput. Se initializeaza culorile si 
perechile de culori. Obtinem dimensiunile ferestrei de terminal. Permitem 
folosirea tastelor speciale. Declaram optiunile meniului.

(2) Definim o bucla infinita pentru joc. Definim o bucla pentru meniu. Stergem 
ecranul pentru a afisa meniul de fiecare data cand ne intoarcem la el.
Verificam daca exista joc in desfasurare, in caz afirmativ, coloram la fel 
optiunea Resume ca New Game si Quit, in caz contrar, coloram cu gri pentru a 
indica ca optiunea Resume nu e disponibila. Citim o tasta pentru a putea 
colora doar tasta ce urmeaza sa fie colorata prin ENTER. Cand se apasa ENTER,
intram in cazul 10 unde abordam optiunea abordata. Daca selectam New Game, 
resetam scorul la 0, desenam tabla si incepem jocul. Verificam daca putem 
selecta Resume (exista un joc inceput), si reapelam functia newGame pentru a 
relua jocul. Daca selectam Quit, iese din program. 

Prezentare subprograme:
(a) drawBoard - deseneaza ecranul jocului principal (tabla de joc, timpul,
 scorul, cel mai bun scor). Pozitioneaza cadranul pe mijlocul ferestrei de terminal.

(b) copy - creeaza o copie tablei de joc, fiecare element fiind adaugat unul 
cate unul.

(c) isSameBoard - verifica daca tablele sunt identice. Compara fiecare element
si returneaza fals daca pe aceeasi pozitie se afla elemente diferite.

(d) endGame - determina daca un joc s-a terminat. Facem o copie a tablei, 
contopim (dupa regula 2048) si mutam fiecare valoare spre directia indicata. 
Daca dupa realizarea mutarii, tabla ramane neschimbata, inseamna ca tasta
respectiva e invalida si o marcam cu 1. Daca toate tastele sunt invalide, 
jocul se termina.

(e) winGame - determina daca un joc este castigat. Daca pe tabla se gaseste
 valoarea 2048, jocul a fost castigat.

(f) bestMove - calculeaza cea mai buna miscare facuta. Aceasta consta in 
efectuarea fiecarei miscari si calcularea spatiilor (celule care retin valoarea 0)
separat. Vedem care miscare a eliberat cele mai multe celule si o transmitem
mai departe. Ordinea fiind A, D, W, S.

(g) showBoard - afiseaza timpul, scorul si cel mai bun scor si coloreaza fiecare 
celula corespunzator cu valoarea pe care o contine.

(h) newGame - functia in care se desfasoara jocul. Se deseneaza tabla, celulele,
se verifica daca nu exista un joc inceput. In caz afirmativ, initializam tabla
cu 0 si obtinem cele 2 numere randomizate din multimea {2, 4}. Marcam inceputul
de joc. Cat timp jocul nu a fost castigat sau pierdut, programul ruleaza.
Daca nicio tasta nu e apasata timp de 1 minut, se aplica miscarea automata 
care foloseste functia bestMove pentru a obtine cea mai "buna" miscare si de a
trece mai departe.
Daca se apasa tasta A, se realizeaza miscarea spre stanga prin contopire si
permutare.
Daca se apasa tasta D, se realizeaza miscarea spre dreapta prin contopire si
permutare. 
Daca s-a modificat tabla, randomizam un numar intr-un spatiu liber.
Daca se apasa tasta S, se realizeaza miscarea in jos prin contopire si permutare. 
Daca s-a modificat tabla, randomizam un numar intr-un spatiu liber.
Daca se apasa tasta H, se deschide fereastra ce contine lista comenzilor 
valide, timpul, scorul si cel mai bun scor.
La finalul fiecarui pas, se verifica daca jocul a fost castigat sau pierdut
utilizand functiile winGame si endGame. In caz afirmativ, afisam un mesaj 
corespunzator.

NOU: Am adaugat un leaderboard care apare la finalul fiecarui joc pierdut sau
castigat.

(i) sortScores - sorteaza scorurile jucatorilor. Formeaza un vector cu jucatorii 
din fisier, ii sorteaza, si rescrie fisierul.

(j) saveHighscores - adauga un jucator la finalul fisierului si dupa apeleaza 
functia sortScores.

(g) - showLeaderboard - afiseaza tabela de scor (primii 5)
==============================================================================.

