# game_of_chance
This program implements a game which uses the concept of pseudo randomization to enable the user to play three different games of chances. 


## usage
Since this is a multi-user program that writes to a file in the /var directory, it must be suid root. 
Therefore to run the game run the following commands in your terminal command:

Compiling command:
```
gcc -o game game.c
```

Making the program suid root:
```
sudo chown root:root .game
```
```
sudo chmod u+s .game
```
