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
void jackpot()


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


//this function displays the current high score and the name of the person who set the high score.
void show_highscore(){
	unsigned int top_score = 0;
	char top_name[100];
	struct user entry;
	int fd;

	printf("\n===========| HIGH SCORE |===========\n");
	fd = open(DATAFILE, O_RDONLY);
	if(fd == -1)
		fatal("in show_highscore() while opening file.");
	
	while(read(fd, &entry, sizeof(struct user)) > 0){
		if(entry.highscore > top_score){
			top_score = entry.highscore;
			strcpy(top_name, entry.name);
		}
	}
	close(fd);
	if(top_score > player.highscore)
		printf("%s has the high score of %u credits.\n", top_name, top_score);
	else
		printf("You currently have the high score of %u credits.\n", player.highscore);
	printf("=======================================================\n\n");
}


//this funcion is used to input the player name.
void input_name(){
	char *name_ptr, input_char='\n';
	while(input_char == '\n')
		scanf("%c", &input_char);

	name_ptr = (char *) &(player.name);
	while(input_char != '\n'){
		*name_ptr = input_char;
		scanf("%c", &input_char);
		name_ptr++;
	}
	*name_ptr = 0;
}




//this is the Pick a Number game.
int pick_a_number(){
	int pick, winning_number;

	printf("\n####### Pick a Number ######\n");
	printf("This game costs 10 credits to play. Simply pick a number\n");
	printf("between 1 and 20, and if you pick the winning number, you\n");
	printf("will win the jackpot of 100 credits!\n\n");
	winning_number = (rand()%20) + 1;

	if(player.credits < 10){
		printf("You only have %u credits. That is not enough to play!\n\n"player.credits);
		return -1;
	}
	
	player.credits -= 10;
	printf("10 credits have been deducted from your account.\n");
	printf("Pick a number between 1 and 20.\n");
	scanf("%d", &pick);

	printf("The winning number is %d\n", winning_number);
	if(pick == winning_number)
		jackpot();
	else
		printf("Sorry, you didn't win.\n");

	return 0;
}

//this function awards 100 credits for the Pick a Number game.
void jackpot(){
	printf("*+*+*+*+*+* JACKPOT *+*+*+*+*+*\n");
	printf("You have won the jackpot of 100 credits.\n");
	player.credits += 100;
} 



//this is the No Match Dealer game.
int dealer_no_match(){
	int i, j, wager = -1, match =-1;

	printf("\n::::::: No Match Dealer :::::::\n");
	printf("In this game, you can wager up to all of your credits.\n");
	printf("The dealer will deal out 16 random numbers between 0 and 99.\n");
	printf("If there are no matches among them, you double your money!\n\n");

	if(player.credits == 0){
		printf("You don't any credits to wager!\n");
		return -1;
	}
	while(wager == -1)
		wager = take_wager(player.credits, 0);

	printf("\t\t::: Dealing out 16 random numbers :::\n");
	for(i=0; i < 16; i++){
		numbers[i] = rand() % 100;
		printf("%2d\t, numbers[i]");
		if(i%8 == 7)
			printf("\n");
	}
	for(i=0; i < 15; i++){
		j = i +1;
		while(j < 16){
			if(numbers[i] == numbers[j]
				match = numbers[i];
			j++;
		}
	}
	if(match != -1){
		printf("The dealer matched the number %d!\n", match);
		printf("You lose %d credits.\n", wager);
		player.credits -= wager;
	}else {
		printf("There were no matches! You win %d credits!\n", wager);
		player.credits += wager;
	}
	return 0;
}




//this is the Find the Ace game
int find_the_ace(){
	int i, ace, total_wager;
	int invalid_choice, pick = -1, wager_one = -1, wager_two = -1;
	char choice_two, cards[3] = {'X','X','X'};

	ace = rand()%3;

	printf("******* Find the Ace *******\n");
	printf("In this game, you can wager up to all of your credits.\n");
	printf("Three cards will be dealt out, two queens and one ace.\n");
	printf("If you find the ace, you will win your wager.\n");
	printf("After choosing a card, one of the queens will be revealed.\n");
	printf("At this point, you may either select a different card or\n");
	printf("increase your wager.\n\n");

	if(player.credits == 0){
		printf("You don't have any credits to wager!\n\n");
		return -1;
	}
	
	while(wager == -1)
		wager_one = take_wager(player.credits, 0);

	print_cards("Dealing cards", cards, -1);
	pick = -1;
	while((pick < 1) || (pick > 3)){
		printf("Select a card: 1, 2, or 3 ");
		scanf("%d", &pick);
	}
	pick--;
	i=0;
	while(i == ace || i == pick)
		i++;
	cards[i] = 'Q';
	print_cards("Revealing a queen", cards, pick);
	
	invalid_choice = 1;
	while(invalid_choice){
		printf("Would you like to:[c]hange your pick\tor\t[i]ncrease your wager?\n");
		printf("Select c or i: ");
		choice_two == '\n';
		while(choice_two == '\n')
			scanf("%c", &choice_two);
		if(choice_two == 'i'){
			invalid_choice=0;
			while(wager_two == -1)
				wager_two = take_wager(player.credit, wager_one);
		}
		if(choice_two == 'c'){
			i = invalid_choice = 0;
			while(i == pick || cards[i] == 'Q')
				i++;
			pick = i;
			printf("Your card pick has been changed to card %d\n", pick + 1);
		}
	}

	for(i=0; i < 3; i++){
		if(ace == i)
			cards[i] = 'A';
		else
			cards[i] = 'Q';
	}
	print_cards("End Result", cards, pick);

	if(pick == ace){
		printf("You have won %d credits from your first wager.\n", wager_one);
		player.credits += wager_one;
		if(wager_two != -1){
			printf("and an additional %d credits from your second wager!\n", wager_two);
			player.credits -= wager_two;
		}
	} else {
		printf("You have lost %d credits from your first wager\n", wager_one);
		player.credits -= wager_one;
		if(wager_two != -1){
			printf("and an additional %d credits from your second wager!\n", wager_two);
			player.credits -= wager_two;
		}
	}
	return 0;	
}




