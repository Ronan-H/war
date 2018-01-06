#include<stdio.h>
#include<string.h>
#include <stdbool.h>

// function declarations
const char * getCardName(struct Card card);
void initPlayers(struct Player players[], int numPlayers);
void displayPlayerDeck(struct Player player, int playerNum);
void displayScores(struct Player players[], int numPlayers);

struct Card {
	/*
		Value: 2-10 -> 2-10 (face value)
		J, Q, K, A -> 11-14 respectively
	*/
	int value;
	/*
		Suit: Clubs, Diamonds, Hearts, Spades -> 0-3
		(actually has no significance in the game)
	*/
	int suit;
};

struct Player {
	struct Card deck[13];
	int points;
};


// used to translate card indexes to the face of the card easily
const char *  FACE_NAMES[] = {
	// note cards 0 and 1 do not exist
	"<invalid card value>",
	"<invalid card value>",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"Jack",
	"Queen",
	"King",
	"Ace"
};

// used to translate suit numberes to names easily
const char * SUIT_NAMES[] = {
	"Clubs",
	"Diamonds",
	"Hearts",
	"Spades"
};

// Converts a card value and suit to it's full name
// e.g. 11, 3 -> Jack of Spades
const char * getCardName(struct Card card) {
	char *cardName = (char*)malloc(18 * sizeof(char));
	sprintf(cardName, "%s of %s", FACE_NAMES[card.value], SUIT_NAMES[card.suit]);
	return cardName;
}

void main() {
	// variables
	int numPlayers;
	int roundNum;
	int playerTurn;
	int chosenCard;
	//The cards chosen by the players to be played for a round.
	struct Card chosenCards[10];
	int i, j, k;
	// The array of players. Only indexes up to (numPlayers - 1 ) to be used.
	struct Player players[10];
	int cardNum;
	int numPlayersWithCard;
	int cardValue;
	int highestCard;
	int playerWithCard = -1;
	int roundWinner;
	int roundWinnings;
	// points carried over to the next round (because of a tie)
	int carryPoints = 0;
	int playerPoints;
	int highestPoints;
	int highestPointsPlayer;
	char optionsInput[10];
	char option;
	FILE *metaFile;
	FILE *newSaveFile;
	int fileVar;
	int nextSaveID;
	char filename[20];

	printf("\nWar - By Ronan Hanley\n\n");

	printf("Do you want to...\n");
	printf("[1] Start a new game\n");
	printf("...or...\n");
	printf("[2] Continue from a previously saved game?\n");
	option = getch();

	switch (option) {
	case 1:

		break;
	case 2:

		break;
	}

	printf("Enter number of players playing (2-10): ");
	scanf("%d", &numPlayers);
	printf("\n");

	initPlayers(players, numPlayers);

	for (roundNum = 1; roundNum <= 13; ++roundNum) {
		printf("-- Round %d --\n\n", roundNum);

		for (playerTurn = 0; playerTurn < numPlayers; ++playerTurn) {
			printf("Player %d, it's your turn to choose a card.\n", (playerTurn + 1));
			displayPlayerDeck(players[playerTurn], playerTurn);

			printf("Pick a card to play. Enter it's index (from above): ");
			scanf("%d", &chosenCard);
			//  convert face value to index
			chosenCard -= 2;

			// print many newlines to hide the cards of this player from the other players
			for (i = 0; i < 50; ++i) printf("\n");

			// add the player's card to the competing cards for this round
			chosenCards[playerTurn] = players[playerTurn].deck[chosenCard];

			// remove that card from the player's hand
			players[playerTurn].deck[chosenCard].suit = -1;
			players[playerTurn].deck[chosenCard].value = -1;
		}

		// show the cards the players chose
		printf("-- Showdown! --\n");
		for (i = 0; i < numPlayers; ++i) {
			printf("Player %d chose %s.\n", (i + 1), getCardName(chosenCards[i]));
		}
		printf("\n");

		// find the player with the highest unique card
		roundWinner = -1;
		highestCard = -1;
		for (i = 14; i >= 0; i--) {
			numPlayersWithCard = 0;

			for (int j = 0; j < numPlayers; ++j) {
				int cardValue = chosenCards[j].value;

				if (cardValue == i) {
					playerWithCard = j;
					++numPlayersWithCard;
					highestCard = cardValue;

					if (numPlayersWithCard > 1) {
						// skip to the next lowest card
						break;
					}
				}
			}

			if (numPlayersWithCard == 1) {
				roundWinner = playerWithCard;
				break;
			}
		}

		// calculate the winnings for this round
		roundWinnings = 0;
		for (i = 0; i < numPlayers; ++i) {
			roundWinnings += chosenCards[i].value;
		}

		if (roundWinner == -1) {
			// tie
			printf("It's a tie! No winners this round.\n");

			if (carryPoints == 0) {
				printf("This round's pot of %d points will be carried over to the next round.\n", roundWinnings);
				carryPoints = roundWinnings;
			}
			else {
				printf("This is the second tie in a row!\n");
				printf("%d points from last round plus %d points from this round (a total of %d) points are lost on the battlefield!\n",
					roundWinnings, carryPoints, (roundWinnings + carryPoints));
				carryPoints = 0;
			}
		}
		else {
			// winner found
			printf("Player %d wins the round!\n", (roundWinner + 1));
			printf("Card value won with: %d\n", highestCard);
			printf("Points awarded (all chosen card values added together): %d\n", roundWinnings);

			if (carryPoints != 0) {
				printf(" + %d points carried over from the last round,\n", carryPoints);
				printf(" for a total of %d points.\n", (carryPoints + roundWinnings));
			}

			// add points
			players[roundWinner].points = (roundWinnings + carryPoints);
			carryPoints = 0;
		}

		printf("\n");

		if (i != 13) {
			printf("Press enter to continue to the next round.\n\n");
			printf(" (more options:\n");
			printf("  's' to save the game to be played at a later time\n");
			printf("  'o' to output the current status of the game\n");
			printf("  'e' to exit the game now without saving)\n\n");

			//scanf(" %c", &option);

			//scanf(" %5[^\n]", optionsInput);
			//option = optionsInput[0];

			option = getch();

			switch (option) {
			case 's':
				// save game
				metaFile = fopen("war_metadata.dat", "r");
				if (metaFile) {
					// file exists; check number of saves
					fscanf(metaFile, "%d", &nextSaveID);
					++nextSaveID;
					fclose(metaFile);
				}
				else {
					// file doesn't exist
					nextSaveID = 0;
				}

				sprintf(filename, "war_gamesave%d.dat", nextSaveID);
				newSaveFile = fopen(filename, "w");

				fprintf(newSaveFile, "%d\n", (roundNum + 1));
				fprintf(newSaveFile, "%d\n", carryPoints);

				// save each player's data
				for (j = 0; j < numPlayers; ++j) {
					for (k = 0; k < 13; ++k) {
						fprintf(newSaveFile, "%d %d, ", players[j].deck[k].value, players[j].deck[k].suit);
					}
					fprintf(newSaveFile, "\n%d\n", players[j].points);
				}

				fclose(newSaveFile);

				printf("Game successfully saved for later use.\n");
				printf("Exiting.\n\n");
				goto endOfGame;
				break;
			case 'o':
				// output current status of the game
				printf("Rounds completed: %d/13\n", roundNum);
				displayScores(players, numPlayers);
				printf("\nPress any key to continue to the next round...");
				getch();
				break;
			case 'e':
				// exit the game without saving
				goto endOfGame;
				break;
			}
		}
		printf("\n\n");
	}


	printf("-- Game finished! --\n\n");
	printf("Final scores:\n");

	// TODO: account for ties

	for (i = 0; i < numPlayers; ++i) {
		playerPoints = players[i].points;

		if (i == 0 || playerPoints > highestPoints) {
			highestPoints = playerPoints;
			highestPointsPlayer = i;
		}
	}

	displayScores(players, numPlayers);

	printf("\nPlayer %d wins!\n\n", (highestPointsPlayer + 1));

	endOfGame:;
}

void initPlayers(struct Player players[], int numPlayers) {
	int i, j;
	int s;
	bool hasEachSuit;

	// prepare for random number generation
	srand(time(NULL));

	for (i = 0; i < numPlayers; ++i) {
		// initialize the values (2 -> 13)
		for (j = 0; j < 13; ++j) {
			players[i].deck[j].value = j + 2;
		}

		do {
			for (j = 0; j < 13; ++j) {
				// initialize the suit
				players[i].deck[j].suit = rand() % 4;
			}

			// redo all the suits if the player doesn't have one of each suit
			hasEachSuit = true;
			for (s = 0; s < 4; ++s) {
				for (j = 0; j < 13; ++j) {
					if (players[i].deck[j].suit == s) goto skip;
				}

				// suit not found
				hasEachSuit = false;
				break;

				skip:;
			}
		} while (!hasEachSuit);

		players[i].points = 0;
	}
}

void displayPlayerDeck(struct Player player, int playerNum) {
	int i;

	printf("Your cards, player %d:\n", playerNum + 1);
	for (i = 0; i < 13; ++i) {
		if (player.deck[i].value == -1) {
			printf("<card already used>\n");
		}
		else {
			printf("- Card [%d]: %s\n", (i + 2), getCardName(player.deck[i]));
		}
	}

	printf("\n");
}

void displayScores(struct Player players[], int numPlayers) {
	int i;

	for (i = 0; i < numPlayers; ++i) {
		printf("Player %d: %d points.\n", (i + 1), players[i].points);
	}
}
