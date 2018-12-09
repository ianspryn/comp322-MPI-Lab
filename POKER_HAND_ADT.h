#pragma once
// Emily Wasylenko ~ COMP322.A ~ Texas Hold'em ADT


#ifndef POKER_HAND_ADT
#define POKER_HAND_ADT


#include <stdio.h>
#include <stdlib.h>
using namespace std;

//1.0 Make struct for our PokerHand
struct PokerHand {
	int num;
	int * hand;
};

//2.0 Function Signature
int royalFlush(PokerHand &myHand);
int isStraightFlush(PokerHand &myHand);
int isFourOfAKind(PokerHand &myHand);
int isFullHouse(PokerHand &myHand);
int isFlush(PokerHand &cards);
int isStraight(PokerHand &myHand);
int threeOfKind(PokerHand &phand);
int two_pair(PokerHand hand);
int isOnePair(PokerHand &Phand);
int isHighCard(PokerHand &pkh);



//3.0 Implement my classmates' (and my own) predicate functions!


//A method by Sarah Calvis 10/21/18
//Checks if a poker hand is a royal flush :)
int royalFlush(PokerHand &cards)
{
	int ret = 1;									//return this
	int suit;										//the suit of a specific card
	int suits[4];									//the number of cards in each suit
	int flushCards[5];								//tells whether each card required for a royal flush is in the hand

	suit = 4;										//will hold the suit of which we have five cards - if it remains four then we don't have five cards of any suit
	suits[4];										//will hold how many cards we have of each suit (each index corresponds to a suit)
	for (int i = 0; i < 4; i++) {					//initialize suits to 0!
		suits[i] = 0;
	}
	for (int i = 0; i < cards.num; i++) {			//loop through cards
		suits[(cards.hand[i] - 1) / 13]++;			//use math to figure out suit of card and record it
	}
	for (int i = 0; i < 4; i++) {					//loop through suits
		if (suits[i] > 4) {							//check if there are more than four cards with the same suit
			suit = i;								//if yes that is the suit that we will check for a royal flush
		}
	}
	if (suit == 4) {								//if suit is still four, than we did not have more than 4 cards in the same suit
		ret = 0;									//so there cannot be a royal flush :(
	}
	for (int i = 0; i < 5; i++) {					//initialize each index of this array to zero
		flushCards[i] = 0;
	}
	for (int i = 0; i < cards.num; i++) {			//loop through the cards
		if ((cards.hand[i] - 1) / 13 == suit) {		//if the card is of the right suit
			if (cards.hand[i] % 13 == 1) {			//if the card is an ace
				flushCards[0] = 1;					//set the ace slot in the flush array to one
			}
			else if (cards.hand[i] % 13 == 10) {	//if the card is a 10
				flushCards[1] = 1;					//set the ten slot in the flush array to one
			}
			else if (cards.hand[i] % 13 == 11) {	//if the card is a jack
				flushCards[2] = 1;					//set the jack slot in the flush array to one
			}
			else if (cards.hand[i] % 13 == 12) {	//if the card is a queen
				flushCards[3] = 1;					//set the queen slot in the flush array to one
			}
			else if (cards.hand[i] % 13 == 0) {		//if the card is a king
				flushCards[4] = 1;					//set the king slot in the flush array to one
			}
		}
	}
	for (int i = 0; i < 5; i++) {					//loop through the flush array
		if (flushCards[i] == 0) {					//if there are any slots still 0
			ret = 0;								//there is no royal flush :(
		}
	}
	return ret;										//the hand has passed every test, it is a royal flush :)
}

//the following define commands were part of Logal Stahl's code
#define DECK_SIZE 52
#define SUIT_SIZE 13
#define NUM_DEALT 7
#define STD_HAND_SIZE 5


/*	Author: Logan Stahl, for COMP322 Fall 2018
**  Receives a PokerHand containing 7 cards
**	Returns whether the hand is a straight flush
*/
int isStraightFlush(PokerHand& cards) {
	int i;								//Loop control variable for indexing through hand
	int suitCts[4] = { 0,0,0,0 };		//The count of each suit in a hand
	int suit;							//Stores the suit of the current card
	int flushSuit;						//Stores the suit of the flush
	int rank;							//The rank of a card
	int numConsec = 1;					//A count of cards in sequence (Always 1 card in sequence)
	int rankCts[14] = { 0 };			//A flag for each rank of card in a hand, init to 0  (14 for high card ace)
	int isFlush = 0;					//Flag for if hand is a flush
	int isStraightFlush = 0;			//Flag for if hand is a staight flush

	//First check if flush
	for (i = 0; i < cards.num; i++) {
		//Use integer division to get suit from range [1...52]
		suit = (cards.hand[i] - 1) / SUIT_SIZE;

		//Increment # of cards in the suit
		suitCts[suit] = suitCts[suit] + 1;

		//Check if enough card for a flush
		if (suitCts[suit] == STD_HAND_SIZE) {
			isFlush = 1;
			flushSuit = suit;
		}
	}

	//Only perform extra work if it's a flush
	if (isFlush) {

		//Create array of flags for if the hand has a rank of card
		for (i = 0; i < cards.num; i++) {
			//We only care about straights within our suit of the flush
			if (flushSuit == ((cards.hand[i] - 1) / SUIT_SIZE)) {
				//Converts range [1...52] to ranks [0...12]
				rank = (cards.hand[i] - 1) % SUIT_SIZE;
				rankCts[rank] = 1;

				//Set a 14th index to 1 for a high card ace
				if (rank == 0) {
					rankCts[13] = 1;
				}
			}
		}

		//Check for consecutive rank cards
		for (i = 0; i < SUIT_SIZE; i++) {
			if (rankCts[i] && rankCts[i + 1]) {
				numConsec++;
				if (numConsec == STD_HAND_SIZE) {
					isStraightFlush = 1;
				}
			}
			else {
				numConsec = 1;
			}
		}

	}//end if(isFlush)

	//Single exit
	return (isFlush && isStraightFlush);
}




/***************************************************
*  Yay, it's my function!						   *
* 												   *
*  This predicate function tells you if			   *
*  you've got 4 cards with the same rank in your   *
*  poker hand!									   *
*												   *
*  Author: Emily Wasylenko						   *
*												   *
****************************************************/
int isFourOfAKind(PokerHand &myHand) {


	//4.0 Variable Dictionary
	bool weGotFour;			//a boolean that checks whether or not there are four of a kind
	int rankAry[13];		//an array that holds the frequency with which each suit appears


	//4.1 Initialize
	weGotFour = false;

	for (int d = 0; d < 13; d++) {
		rankAry[d] = 0;
	}


	//4.2 Increment the calculated suit seen in each hand
	for (int h = 0; h < 7; h++) {
		rankAry[myHand.hand[h] % 13]++;
	}


	//4.3 after the switch/case, test if any rank counter = 4
	//if, at any point, we had 4 and 4 only, make our boolean TRUE!
	for (int checker = 0; checker < 13; checker++) {
		if (rankAry[checker] == 4) {
			weGotFour = true;
		}
	}

	//4.4 Deliver unto the user what was promised
	//return weGotFour;

	if (weGotFour) {
		return 1;
	}
	else {
		return 0;
	}

}


// Mitchell Smith - COMP 322 - Oct 22, 2018 - Texas Holdem Full House Function

// isFullHouse
//
// Takes a poker hand and returns true if it contains a full house.
// Works with any number of cards in hand.
int isFullHouse(PokerHand &myHand) {
	int rankCounts[13];				// Stack allocated array for counting the same ranks
	int pairCounts[5];				// Stack allocated array for counting the same pair numbers (2 pair, 3 of a kind, etc.)
	for (int i = 0; i < 13; i++) {	// Makes ure rankCounts are all set to 0
		rankCounts[i] = 0;
	}
	for (int i = 0; i < 5; i++) {	// Makes ure pairCounts are all set to 0
		pairCounts[i] = 0;
	}

	// Counts number of same ranks together
	for (int card = 0; card < myHand.num; card++) {
		rankCounts[(myHand.hand[card] - 1) % 13]++;
	}

	// Counts number of same pairs together
	for (int rank = 0; rank < 13; rank++) {
		pairCounts[rankCounts[rank]]++;
	}

	// If there's a 4 pair, this is not a full house
	if (pairCounts[4] > 0) {
		return 0;
	}
	//If there's 2 or more three pairs, this is a full house
	if (pairCounts[3] > 1) {
		return 1;
	}
	//If there's 1 three pair, make sure there's at least one two pair
	if (pairCounts[3] == 1 && pairCounts[2] > 0) {
		return 1;
	}
	// False otherwise
	return 0;
}




//Peter Lowrance - COMP322 - Part 5 IsFlush
//This file contains a function for checking if a poker hand is a flush
#define NUM_CARDS 7

//Takes a PokerHand
//Returns 1 if the poker hand is a flush and 0 otherwise
int isFlush(PokerHand &cards) {
	int numEachSuit[4] = { 0 }; //variable to store number of cards of different suit

	//loop to check if each card until we find one with a different suit
	for (int i = 0; i < NUM_CARDS; i++) {
		//check if this card and the next one are the same suit using integer division
		numEachSuit[(cards.hand[i] - 1) / 13]++;
	}
	for (int i = 0; i < 4; i++) {
		if (numEachSuit[i] >= 5) {
			return 1;
		}
	}
	return 0;
}


//6
//Written by Sarah Calvis
//Takes a poker hand
//checks if the hand is a straight
//outputs 0 for not a straight, one for is a straight
int isStraight(PokerHand &phand) {
	int yes;                      //a check we will use later
	int ret = 0;                    //return value

	for (int i = 0; i < phand.num; i++) {        //loop through cards
		int count[5] = { 1,0,0,0,0 };          //an array of five cards holding whether or not cards in a flush are present
		for (int j = 0; j < phand.num; j++) {      //loop through cards again
			if (j != i) {                //skip the card we put in slot 1 of array

				if (phand.hand[i] % 13 == 0) {      //if the card is an ace, take precautions because ace is rep. by 0 but is the highest card
				  //check if the cards consecutive to ace are present
					if (phand.hand[j] % 13 == 12) {
						count[4] = 1;
					}
					else if (phand.hand[j] % 13 == 11) {
						count[3] = 1;
					}
					else if (phand.hand[j] % 13 == 10) {
						count[2] = 1;
					}
					else if (phand.hand[j] % 13 == 9) {
						count[1] = 1;
					}
				}
				//if card is not ace, check if cards consecutive to it are present
				else if (phand.hand[i] % 13 == phand.hand[j] % 13 + 1) {
					count[1] = 1;
				}
				else if (phand.hand[i] % 13 == phand.hand[j] % 13 + 2) {
					count[2] = 1;
				}
				else if (phand.hand[i] % 13 == phand.hand[j] % 13 + 3) {
					count[3] = 1;
				}
				else if (phand.hand[i] % 13 == phand.hand[j] % 13 + 4) {
					count[4] = 1;
				}
			}

			//check if the array is all 1s
			yes = 1;
			for (int i = 0; i < 5; i++) {
				if (count[i] == 0) {
					yes = 0;
				}
			}
			//if it is set the return value to true
			if (yes == 1) {
				ret = 1;
			}
		}
	}

	//return
	return ret;
}


//Writen by Samuel Beiler 10/18/18
//Modified by Emily Wasylenko 10/24/18
//Checks if a poker hand is a three of a kind
int threeOfKind(PokerHand &phand) {
	int count[13] = {};//Holds count of each card number, initialized to 0
	int number; //Holds cards number, disregarding suit
	for (int i = 0; i < phand.num; i++)
	{
		number = phand.hand[i] % 13;//Calc. card's number
		count[number]++;
		if (count[number] == 3)//Checks if count has hit 3
		{
			return 1;
		}
	}
	return 0;
}


//Noah Rosamilia
//Repaired by Sarah Calvis
int two_pair(PokerHand hand) {
	int values[13] = { 0 };
	for (int i = 0; i < 7; i++) {
		values[hand.hand[i] % 13]++;
	}

	int pairs = 0;
	for (int i = 0; i < 13; i++) {
		if (values[i] >= 2) {
			pairs++;
		}
	}

	return pairs >= 2;
}


//This method will check to see if two cards share a similar number
//It will return 1 if that case is true, 0 otherwise
int isOnePair(PokerHand &Phand) {
	int onePair = 0; //Variable to determine if the hand has a one-pair
	int card = 0; //Card to be checked

	//This will be assuming that the cards are sorted in ascending order
	//This nested for-loop will check if the hand has two cards of the same number
	for (int i = 0; i < (Phand.num - 1); i++) {
		card = Phand.hand[i];
		for (int j = i; j < Phand.num; j++) {
			if (Phand.hand[j] == (card + 13) || Phand.hand[j] == (card + 26) || Phand.hand[j] == (card + 39)) {
				onePair = 1;
			}
		}
	}
	return onePair;
}//One-Pair Check: Nathan Stoner

//Author: Emily Wasylenko
int isHighCard(PokerHand &pkh) {
	return 1;
}


#endif //POKER_HAND_ADT end inclusion guard