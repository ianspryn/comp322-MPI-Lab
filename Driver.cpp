// Emily Wasylenko ~ COMP322.A ~ Texas Hold'Em Driver


#include <stdio.h>
#include <stdlib.h>
#include "POKER_HAND_ADT.h"
#include <omp.h
#include <mpi.h>
using namespace std;


//member variables
int source, myRank, nodeSize = 0


/************************************************
** This is a driver for our Poker Hand ADT.
** It generates every possible combination of
** 7 cards and prints the probability of each
** of the 10 poker hands in Texas Hold'Em.
************************************************/
int main() {

	printf("COMP322.D ~ Texas Hold'Em\n");
	printf("*** Calculate Probability for all 10 Texas Hold'Em Hands ***\n\n");

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &nodeSize);



	//1.0 Initialize everything!

	//1.1 init poker hand ADT struct variables
	PokerHand pkh;	//Poker hand object
	const int num = 7;		//number of cards in the hand
	int cards[num];	//an array of cards in the hand

	//1.2 init. probability calculators
	int totalRuns = 0;

	//1.3 init slave counters
	int cntr1, cntr2, cntr3, cntr4, cntr5, cntr6, cntr7, cntr8, cntr9, cntr10 = 0;
	//1.4 init reduce counters
	int totalCntr1, totalCntr2, totalCntr3, totalCntr4, totalCntr5, totalCntr6, totalCntr7, totalCntr8, totalCntr9, totalCntr10 = 0;

	//1.5 define loop variables
	int a, b, c, d, e, f, g;

	//SET THREAD NUMBER HERE
	int threadNum = 4;
	omp_set_num_threads(threadNum);


	//Master does outermost loop...
	//sends work to the ancillaries...

	//If(you're the master node) {
	if (myRank == 0) {
		//	Run “a” = 0– > 46
		//	Send “a” to the slaves
		for (a = 0; a < 46; a++) {
			MPI_Send(a, 1, MPI_Int, 1, 0, MPI_COMM_WORLD);
		}

	} else {
		
		//	  Receive “a”
		MPI_Recv(a, 1, MPI_Int, 0, 0);
		
		//	  Init a struct
		PokerHand pkh;	//Poker hand object
		const int num = 7;		//number of cards in the hand
		int cards[num];	//an array of cards in the hand


		pkh = { num, cards };
		for (int i = 0; i < num; i++) {
			cards[i] = 0;
		}



		//	  Run 7 inner for loops
#pragma omp parallel for num_threads(threadNum) schedule(dynamic, 1) default(none) \
private(pkh, cards) \
reduction(+:totalRuns, cntr1, cntr2, cntr3, cntr4, cntr5, cntr6, cntr7, cntr8, cntr9, cntr10)

		for (b = a + 1; b < 47; b++) {

			for (c = b + 1; c < 48; c++) {

				for (d = c + 1; d < 49; d++) {

					for (e = d + 1; e < 50; e++) {

						for (f = e + 1; f < 51; f++) {

							for (g = f + 1; g < 52; g++) {		//card # 52 inclusive


								totalRuns++;


								pkh.hand[0] = a + 1;
								pkh.hand[1] = b + 1;
								pkh.hand[2] = c + 1;
								pkh.hand[3] = d + 1;
								pkh.hand[4] = e + 1;
								pkh.hand[5] = f + 1;
								pkh.hand[6] = g + 1;

								//3.0 Test all 10 functions, increment the counter for each function!
								//Reduce on these somehow

								if (royalFlush(pkh)) {
									cntr1++;
								}
								else if (isStraightFlush(pkh)) {
									cntr2++;
								}
								else if (isFourOfAKind(pkh)) {
									cntr3++;
								}
								else if (isFullHouse(pkh)) {
									cntr4++;
								}
								else if (isFlush(pkh)) {
									cntr5++;
								}
								else if (isStraight(pkh)) {
									cntr6++;
								}
								else if (threeOfKind(pkh)) {
									cntr7++;
								}
								else if (two_pair(pkh)) {
									cntr8++;
								}
								else if (isOnePair(pkh)) {
									cntr9++;
								}
								else if (isHighCard(pkh)) {
									cntr10++;
								}
							}
						}
					}
				}
			}
		}
		


	}//end else

	//REDUCE
	MPI_Reduce(&cntr1, &totalCntr1, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr2, &totalCntr2, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr3, &totalCntr3, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr4, &totalCntr4, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr5, &totalCntr5, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr6, &totalCntr6, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr7, &totalCntr7, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr8, &totalCntr8, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr9, &totalCntr9, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&cntr10, &totalCntr10, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);




	//4.0 Print the probability of each hand
	printf("\nProbability of getting Royal Flush: %f", 100 * double(totalCntr1) / double(totalRuns));
	printf("%%\nProbability of getting Straight Flush: %f", 100 * double(totalCntr2) / double(totalRuns));
	printf("%%\nProbability of getting Four Of A Kind: %f", 100 * double(totalCntr3) / double(totalRuns));
	printf("%%\nProbability of getting Full House: %f", 100 * double(totalCntr4) / double(totalRuns));
	printf("%%\nProbability of getting Flush: %f", 100 * double(totalCntr5) / double(totalRuns));
	printf("%%\nProbability of getting Straight: %f", 100 * double(totalCntr6) / double(totalRuns));
	printf("%%\nProbability of getting Three of a Kind: %f", 100 * double(totalCntr7) / double(totalRuns));
	printf("%%\nProbability of getting Two Pairs: %f", 100 * double(totalCntr8) / double(totalRuns));
	printf("%%\nProbability of getting One Pair: %f", 100 * double(totalCntr9) / double(totalRuns));
	printf("%%\nProbability of getting the High Card: %f", 100 * double(totalCntr10) / double(totalRuns));
	printf("%%\n");


	MPI_Finalize();

}
//end main
