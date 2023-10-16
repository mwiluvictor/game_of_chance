#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "game.h" 



struct user{
	int uid;
	int credits;
	int highscore;
	char name[100];
	int (*current_game) ();

};



//Function Prototypes

int get_player_data();
void register_new_player();



//Global Variable
struct user player;



//Main Function
int main(){
	int choice, last_game;
	srand(time(0));

	if(get_player_data()==-1)
		register_new_player();


}
