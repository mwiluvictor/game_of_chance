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
int pick_a_number();
int dealer_no_match();
int find_the_ace();
void show_highscore();
void input_name();
void update_player_data();



//Global Variable
struct user player;



//Main Function
int main(){
	int choice, last_game;
	srand(time(0));

	if(get_player_data()==-1)
		register_new_player();


	while(choice != 7){
		printf("1. Play the pick a number game.\n");
		printf("2. Play the no match dealer game.\n");
		printf("3. Play the find the ace game.\n");
		printf("4. View current highscore.\n");
		printf("5. Change user name.\n");
		printf("6. Reset the account at 100 credits.\n");
		printf("7. Quit!\n");
		printf("Player Name: %s\n", player.name);
		printf("[You have %u credits] -> ", player.credits);
		scanf("%d", &choice);
	

		if((choice < 1 || choice > 7))
			printf("\n Error: The entered number %d is an invalid selection.\n\n", choice);
		else if(choice < 4){
			if(choice == 1)
				player.current_game = pick_a_number;
			else if(choice == 2)
				player.current_game = dealer_no_match;
			else 
				player.current_game = find_the_ace;
			last_game = choice;	
		}
		else if(choice == 4)
			show_highscore();
		else if(choice == 5){
			printf("\nChange user name\n");
			printf("Enter your new name: ");
			input_name();
			printf("Your name has been changed.\n\n");
		}
		else if(choice == 6)
			printf("\nYour account has been reset with 100 credits.\n\n");
			player.credits = 100;
	}
	update_player_data();
	printf("\nThanks for playing! Adios\n");

}


//this function reads the player data for the current uid.
int get_player_data(){
	int fd, uid, read_bytes;
	struct user entry;

	uid = getuid();

	fd = open(DATAFILE, O_RDONLY);

	if(fd == -1)
		return -1;
	
	read_bytes = read(fd, &entry, sizeof(struct user));
	while(entry.uid != uid && read_bytes > 0){
		read_bytes = read(fd, &entry, sizeof(struct user));
	}
	close(fd);
	if(read_bytes < sizeof(struct user))
		return -1;
	else
		player = entry;

	return 1;
	
}


//this function registers a new player with no prior data and creates a new player account.
void register_new_player(){
	int fd;

	printf("***New player registration***\n");
	printf("Please enter your name: \n");
	input_name();

	fd = open(DATAFILE, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
	if(fd == -1)
		fatal("in register_new_player() while opening file");
	write(fd, &player, sizeof(struct user));
	close(fd);

	printf("***Welcome to the Game of Chance***\n");
	printf("You have been given %u credits.\n", player.credits);

}


//this function writes the player data to file updating the credits after the game.
void update_player_data(){
	int fd, i, read_uid;
	char burnt_byte;

	fd = open(DATAFILE, O_RDWR);
	if(fd == -1)
		fatal("in update_player_data() while opening file.")
	
	read(fd, &read_uid, 4);
	while(read_uid != player.uid){
		for(i=0; i < sizeof(struct user) - 4; i++)
			read(fd, &read_uid, 1);
		read(fd, &read_uid, 4);
	}

	write(fd, &(player.credits), 4);
	write(fd, &(player.highscore), 4);
	write(fd, &(player.name), 100);
	close(fd);
}
