//Texas-Serial.c
// COMP322.A ~ Texas Hold'Em Driver "Serial"
// (we specified this to run with 1 node in our mpirun command: "mpirun -np 1 ...etc")
// Team F: Ian Spryn, Emily Wasylenko, Mitchell Harvey, Kaileigh Macleod


#include <stdio.h>
#include <stdlib.h>
#include "POKER_HAND_ADT.h"
#include <omp.h>
#include <mpi.h>
using namespace std;


//member variables
int source, myRank, nodeSize = 0;


/************************************************
 ** This is a driver for our Poker Hand ADT.
 ** It generates every possible combination of
 ** 7 cards and prints the probability of each
 ** of the 10 poker hands in Texas Hold'Em. 
 ************************************************/
int main() {

    printf("*** Calculate Probability for all 10 Texas Hold'Em Hands ***\n\n");

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &nodeSize);

    int i = 0;

    //1.0 Initialize everything!
    // int threadNum = 4;
    // omp_set_num_threads(threadNum);

    //1.1 init poker hand ADT struct variables
    const int numCards = 7;
    struct PokerHand pkh;	//Poker hand object
    pkh.num = numCards;
    pkh.hand = new int[pkh.num];

    //1.2 init. probability calculators
    int totalRuns = 0;

    //1.3 init slave counters and ready vars
    int readyInt = 0;
    int cntr1 = 0;
    int cntr2 = 0;
    int cntr3 = 0;
    int cntr4 = 0;
    int cntr5 = 0;
    int cntr6 = 0;
    int cntr7 = 0;
    int cntr8 = 0;
    int cntr9 = 0;
    int cntr10 = 0;

    //1.4 init reduce counters
    int totalCntr1 = 0;
    int totalCntr2 = 0;
    int totalCntr3 = 0;
    int totalCntr4 = 0;
    int totalCntr5 = 0;
    int totalCntr6 = 0;
    int totalCntr7 = 0;
    int totalCntr8 = 0;
    int totalCntr9 = 0;
    int totalCntr10 = 0;
    int totalTotalRuns = 0;

    //1.5 initialize loop variables
    int a, b, c, d, e, f, g;
    a = 1;

    //2.0 Run Loops
    //Master does outermost loop...
    //sends work to the ancillaries...

    //If(you're the master node) {
    if (myRank == 0) {
        //	Run a = 0 -> 46
        //	Send a to the slaves
        for (a = 0; a < 46; a++) {
            //MPI_Recv(void* data,int count,MPI_Datatype datatype,int source,int tag,MPI_Comm communicator,MPI_Status* status)
            MPI_Recv(&readyInt, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Received from: %d\n", readyInt);
            //MPI_Send(void* data,int count,MPI_Datatype datatype, int destination,int tag, MPI_Comm communicator)
            MPI_Send(&a, 1, MPI_INT, readyInt, 0, MPI_COMM_WORLD);
        }

        //start for loop to close down ancillary processes
        for(i = 1; i<nodeSize; i++){
            a = -1;
            MPI_Recv(&readyInt, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Going to tell node %d to finalize.\n", readyInt);
            MPI_Send(&a, 1, MPI_INT, readyInt, 0, MPI_COMM_WORLD);
        }

    } else {

        while(a >= 0){
            readyInt = myRank;
            MPI_Send(&readyInt, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Node %d received a value: %d\n", myRank, a);
            if (a < 0) {
                break;
            }

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
            }
        }
    

    //REDUCE
    printf("node %d: %d\n", myRank, cntr1);
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
    MPI_Reduce(&totalRuns, &totalTotalRuns, nodeSize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    if (myRank == 0) {
        //4.0 Print the probability of each hand

        printf("Cntr1: %d\n", totalCntr1); 
        printf("Cntr2: %d\n", totalCntr2); 
        printf("Cntr3: %d\n", totalCntr3); 
        printf("Cntr4: %d\n", totalCntr4); 
        printf("Cntr5: %d\n", totalCntr5); 
        printf("Cntr6: %d\n", totalCntr6); 
        printf("Cntr7: %d\n", totalCntr7); 
        printf("Cntr8: %d\n", totalCntr8); 
        printf("Cntr9: %d\n", totalCntr9); 
        printf("Cntr10: %d\n", totalCntr10); 
        printf("totalRuns: %d\n", totalTotalRuns); 

        printf("Probability of getting Royal Flush:     %2.4f%%\n", 100.0 * (double) totalCntr1 / (double) totalTotalRuns);
        printf("Probability of getting Straight Flush:  %2.4f%%\n", 100.0 * (double) totalCntr2 / (double) totalTotalRuns);
        printf("Probability of getting Four Of A Kind:  %2.4f%%\n", 100.0 * (double) totalCntr3 / (double) totalTotalRuns);
        printf("Probability of getting Full House:      %2.4f%%\n", 100.0 * (double) totalCntr4 / (double) totalTotalRuns);
        printf("Probability of getting Flush:           %2.4f%%\n", 100.0 * (double) totalCntr5 / (double) totalTotalRuns);
        printf("Probability of getting Straight:        %2.4f%%\n", 100.0 * (double) totalCntr6 / (double) totalTotalRuns);
        printf("Probability of getting Three of a Kind: %2.4f%%\n", 100.0 * (double) totalCntr7 / (double) totalTotalRuns);
        printf("Probability of getting Two Pairs:       %2.4f%%\n", 100.0 * (double) totalCntr8 / (double) totalTotalRuns);
        printf("Probability of getting One Pair:        %2.4f%%\n", 100.0 * (double) totalCntr9 / (double) totalTotalRuns);
        printf("Probability of getting the High Card:   %2.4f%%\n", 100.0 * (double) totalCntr10 / (double) totalTotalRuns);
    }


    MPI_Finalize();	  
    return 0;
}