#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#define WLSIZE 100
#define WORDSIZE 50

//App title
char *title = "Jogo da Forca";

//Screen size
short int row,col;

char wordlist[WLSIZE][WORDSIZE];//Word list

char word[WORDSIZE];//The sorted and correct word

/* Auxiliar third party delay function - http://c-for-dummies.com/blog/?p=69 */
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}


//Print centered function
void printCentered(int row, int totalcols, char *string2print){
	move(row, 0);
	clrtoeol();
	mvprintw(row, (totalcols-strlen(string2print))/2,"%s\n", string2print);
}
void printCenteredDec(int row, int totalcols, char *string2print, char decorationChar){
	short int _prtcnt_i;
	//Print the decoration char above title
	for(_prtcnt_i = 0; _prtcnt_i < ((totalcols-strlen(string2print))/2)-1; _prtcnt_i++){
		mvprintw(row, _prtcnt_i, "#");
	}
	//Print title in the center
	mvprintw(row, (totalcols-strlen(string2print))/2,"%s\n", string2print);
	//Print the decoration char after title
	for(_prtcnt_i = _prtcnt_i + strlen(string2print) + 2; _prtcnt_i < col; _prtcnt_i++){
		mvprintw(row, _prtcnt_i, "#");
	}
}

//Splash with ASCII art
void splashScreen(){
	curs_set(0);
	mvprintw(3, 0, "      #                                       ######                            "); 
	mvprintw(4, 0, "      #  ####   ####   ####   #####    ##     #       ####  #####   ####    ##  "); 
	mvprintw(5, 0, "      # #    # #    # #    #  #    #  #  #    #      #    # #    # #    #  #  # "); 
	mvprintw(6, 0, "      # #    # #      #    #  #    # #    #   #####  #    # #    # #      #    #"); 
	mvprintw(7, 0, "#     # #    # #  ### #    #  #    # ######   #      #    # #####  #      ######"); 
	mvprintw(8, 0, "#     # #    # #    # #    #  #    # #    #   #      #    # #   #  #    # #    #"); 
	mvprintw(9, 0, " #####   ####   ####   ####   #####  #    #   #       ####  #    #  ####  #    #"); 
	refresh();
	delay(2000);
	curs_set(1);
}

//Show header with title
void showHeader(){
	//Clear screen
	wclear(stdscr);
	//Declare i for counter
	short int _clrscr_i;
	//Print the first divisor line
	for(_clrscr_i = 0; _clrscr_i < col; _clrscr_i++){
		mvprintw(0,_clrscr_i,"-");
	}
	//Print the # above title
	for(_clrscr_i = 0; _clrscr_i < ((col-strlen(title))/2)-1; _clrscr_i++){
		mvprintw(1,_clrscr_i,"#");
	}
	//Print title in the center
	mvprintw(1, (col-strlen(title))/2,"%s\n", title);
	//Print the # after title
	for(_clrscr_i = _clrscr_i + strlen(title) + 2; _clrscr_i < col; _clrscr_i++){
		mvprintw(1, _clrscr_i, "#");
	}
	//Print a divisor line
	for(_clrscr_i = 0;_clrscr_i < col; _clrscr_i++){
		printw("-");
	}
}

//Get wordlist from file
void getWords(char * listname){
	//Initiliaze file name and mount it
	char listname_file[100] = "";
	strcat(listname_file, "./wordlists/");
	strcat(listname_file, listname);
	strcat(listname_file, ".wordlist.tdb");
	//Open the file in read mode
	FILE *fp = fopen(listname_file, "r");
	for(int i=0; i < WLSIZE; i++){
		//Clear wordlist
		strcpy(wordlist[i], "");
	}
	int i=0;
	char line[WORDSIZE];
	while (fgets(line, sizeof(line), fp) != NULL){
		//Read word list db file i line into buffer string
		strcpy(wordlist[i], line);
		wordlist[i][strlen(wordlist[i])-1] = (char) '\0';
		i++;
	}
	//Close file
	fclose(fp);
}

//Choose dir
void chooseList(){
	DIR *dp;
	struct dirent *ep;

	char wordlists[100][100];
	//Clear array
	for(int i=0; i<100; i++){
		strcpy(wordlists[i], "");
	}

	//Open dir with wordlists
	dp = opendir("./wordlists");
	int i=0;
	//Read file by file on dir
	while(ep = readdir(dp)){
		//Check if the file name is big enough to have our ext
		if(strlen(ep->d_name) - 13 > 1){
			//Check if the file extension is .wordlist.tdb
			if(!strcmp(ep->d_name + strlen(ep->d_name) - 13, ".wordlist.tdb")){
				//Copy to array
				strncpy(wordlists[i], ep->d_name, strlen(ep->d_name) - 13);
				//Run the counter
				i++;
			}
		}
	}
	(void) closedir(dp);

	//Run by the array, offering the options
	for(int i=0; i<100; i++){
		//If it's not blank, then...
		if(strlen(wordlists[i]) > 0){
			//Check if it's for be shown on left, or right
			int colToShow = 0;
			//if(i%2 == 0)
			colToShow = ((i+1)%2 == 0)? col/2 : 0;
			//Show it
			mvprintw((i/2)+5, colToShow, "%d->%s", i, wordlists[i]);
		}
	}
	//Ask for the choosen option
	move(4, 0); clrtoeol();
	mvprintw(4, 0, "Escolha a categoria de palavra:");

	//Get word category
	char str_catId[10];
	//Clear category id
	strcpy(str_catId, "");
	//Get the category id while it's blank
	while(strlen(str_catId) <= 0){
		getstr(str_catId);
	}
	//Cast to int
	int catId = atoi(str_catId);

	//Load the words in file
	getWords(wordlists[catId]);
}

//Random word
void randWord(){
	srand(time(NULL));
	char choosenWord[WORDSIZE] = "";
	while(strlen(choosenWord) <= 0){
		strcpy(choosenWord, wordlist[rand() % WLSIZE]);
	}
	strcpy(word, choosenWord);
}

//Show word
void showWord(char *wordToShow){
	mvprintw(5, col/2, "%s", wordToShow);
}

//Check if the game is won
bool checkWin(char * correctWord, char * playingWord){
	char temp[WORDSIZE];
	bool result = 1;
	for(int i=0; i<strlen(correctWord); i++){
		if(correctWord[i] != playingWord[(i*2)+1]){
			result = 0;
		}
	}
	return(result);
}

//Try to play with the letter
bool tryPlay(char letter, char * playingWord, char * correctWord){
	bool success = 0;
	for(int i=0; i < strlen(correctWord); i++){
		if(correctWord[i] == letter){
			playingWord[(i*2)+1] = letter;
			success = 1;			
		}
	}
	return(success);
}

//Draw ASCII art
void drawArt(unsigned short int chances){
	if(chances<=1){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |   O       *\n");
	    mvprintw(10, col/2, "\t*         |  /|\\      *\n");
	    mvprintw(11, col/2, "\t*         |   |       *\n");
	    mvprintw(12, col/2, "\t*         |  /        *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
	if(chances==2){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |   O       *\n");
	    mvprintw(10, col/2, "\t*         |  /|\\      *\n");
	    mvprintw(11, col/2, "\t*         |   |       *\n");
	    mvprintw(12, col/2, "\t*         |           *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
	if(chances==3){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |   O       *\n");
	    mvprintw(10, col/2, "\t*         |  /|\\      *\n");
	    mvprintw(11, col/2, "\t*         |           *\n");
	    mvprintw(12, col/2, "\t*         |           *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
	if(chances==4){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |   O       *\n");
	    mvprintw(10, col/2, "\t*         |  /|       *\n");
	    mvprintw(11, col/2, "\t*         |           *\n");
	    mvprintw(12, col/2, "\t*         |           *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
	if(chances==5){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |   O       *\n");
	    mvprintw(10, col/2, "\t*         |           *\n");
	    mvprintw(11, col/2, "\t*         |           *\n");
	    mvprintw(12, col/2, "\t*         |           *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
	if(chances>=6){
		mvprintw(7 , col/2, "\t*          ___        *\n");
	    mvprintw(8 , col/2, "\t*         |   |       *\n");
	    mvprintw(9 , col/2, "\t*         |           *\n");
	    mvprintw(10, col/2, "\t*         |           *\n");
	    mvprintw(11, col/2, "\t*         |           *\n");
	    mvprintw(12, col/2, "\t*         |           *\n");
	    mvprintw(13, col/2, "\t*         |______     *\n");
	    mvprintw(14, col/2, "\t*                     *\n");
	}
}
//Refresh the game
void refreshGame(char * playingWord, int chances){
	char txtChances[30] = "Você tem ";
	char chancesStr[2] = {chances + '0', '\0'};
	strcat(txtChances, chancesStr);
	strcat(txtChances, " chances");
	move(4, 0); clrtoeol();//Clear line
	printCentered(4, col, txtChances);//Print the chances centered
	move(13, 0); clrtoeol();//Clear line
	mvprintw(13, 4, playingWord);//Print the word centered

	drawArt(chances);//Draw
}

//Check for repeated letters
bool checkRepeated(char letter, char * repeatedLetters){
	bool result = 0;
	//Run by repeated letters
	for(int i=0; i<50; i++){
		//If it's equal to this letters, set true
		if(letter == repeatedLetters[i]){
			result = 1;
		}
	}
	return result;
}

int main(){
	setlocale(LC_ALL, "ptb");//Set pt-br
	initscr();//Initialize cursor mode
	start_color();//Initialize color mode
	getmaxyx(stdscr,row,col);//Get screen dimensions
	attron(A_BOLD);//Enable bold


	//Splash screen
	splashScreen();

	//$playAgain, for play again loop
	char playAgain = 's';
	//Loop while the answer for play again question on the end is positive
	while(playAgain == 's'){
		showHeader();//Clear and show header

		chooseList();//Ask user to choose the wordlist

		showHeader();//Clear and show header

		randWord();//Randomize the word

		//Declare the playing word
		char playingWord[WORDSIZE*2] = "";
		//Fill the word in game with spaces
		for(int i=0; i<strlen(word); i++)
			strcat(playingWord, " _");

		int chances = 6;

		//Repeated letters
		char repeatedLetters[50] = "";

		while(!checkWin(word, playingWord) && chances >0){
			//Refresh game table
			refreshGame(playingWord, chances);
			//Request a match
			move(7, 0); clrtoeol();
			mvprintw(7, 0, "Digite a próxima letra: ");
			drawArt(chances);
			move(7, 24);
			char letter = tolower(getch());
			while(checkRepeated(letter, repeatedLetters)){
				move(7, 0); clrtoeol();
				mvprintw(7, 0, "A letra já foi escolhida, tente outra: ");
				drawArt(chances);
				move(7, 39);//Move back to end of asking line
				letter = tolower(getch());
			}
			//Add letter to repeated
			char letterStr[2] = {letter, '\0'};
			strcat(repeatedLetters, letterStr);
			//repeatedLetters[strlen(repeatedLetters)+1] = letter;
			//Try to play
			if(!tryPlay(letter, playingWord, word))
				chances--;
		}

		refreshGame(playingWord, chances);//Refresh game table

		curs_set(0);//Disable cursor

		showHeader();//Clear and show header
		mvprintw(3, 0, "Fim de jogo!\nA palavra era \"%s\".", word);
		if(checkWin(word, playingWord)){
			mvprintw(6, 0, "Você acertou!");
		}else{
			mvprintw(6, 0, "Você perdeu!");
		}

		playAgain = ' ';
		while(playAgain != 'n' && playAgain != 's'){
			move(8, 0); clrtoeol();//Clear line
			mvprintw(8, 0, "Deseja jogar novamente [s/n]? ");
			playAgain = getch();
		}
	}

	//Clear screen and show the header
	showHeader();
	//Print the thanks on center
	mvprintw(6, (col-strlen("Obrigado por jogar!"))/2,"Obrigado por jogar!\n");
	//Delay
	refresh(); delay(600);
	//Print the credits
	mvprintw(9, 0, "Desenvolvido por:\n");
	printw("Renan Galeno <renangaleno@gmail.com>\n");
	printw("Marcos Antuanny <caramarcos@hotmail.com>\n");
	printw("Victor Patrick <vp8_10@hotmail.com>\n");
	printw("\n");
	//Delay
	refresh(); delay(600);
	printw("Este trabalho está licenciado sob uma Licença Creative Commons Atribuição-CompartilhaIgual 4.0 Internacional.\nPara ver uma cópia desta licença, visite http://creativecommons.org/licenses/by-sa/4.0/.\n\n");
	//Delay
	refresh(); delay(1000);
	//Print the press anything to exit message
	printw("Pressione qualquer tecla para sair.");
	getch();

	endwin();//End curses mode
	
	return(0);
}