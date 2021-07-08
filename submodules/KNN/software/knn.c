#include "system.h"
#include "periphs.h"
#include <iob-uart.h>
#include "timer.h"
#include "iob_knn.h"
#include "random.h" //random generator for bare metal
#include "printf.h" 
#include "stdint.h"
#include "KNNsw_reg.h"
#include "interconnect.h"

//uncomment to use rand from C lib 
#define cmwc_rand rand
/*********************************NOT USED NOW************/
//square distance between 2 points a and b
//unsigned int sq_dist( struct datum a, struct datum b) {
//  short X = a.x-b.x;
//  unsigned int X2=X*X;
//  short Y = a.y-b.y;
//  unsigned int Y2=Y*Y;
//  return (X2 + Y2);
//}
/*********************************NOT USED NOW************/
//insert element in ordered array of neighbours
/*void insert (struct neighbor element, unsigned int position) {
  for (int j=K-1; j>position; j--)
    neighbor[j] = neighbor[j-1];

  neighbor[position] = element;

}*/
//Base address of KNN HW

int main() {

  unsigned long long elapsed;
  unsigned long long elapsedua;
  unsigned long long elapsedub;

  //init uart and timer
  uart_init(UART_BASE, FREQ/BAUD);
  printf("\nInit timer\n");
  uart_txwait();

  timer_init(TIMER_BASE);
  //read current timer count, compute elapsed time
  //elapsed  = timer_get_count();   
  int votes_acc[C] = {0};
  //generate random seed 
  random_init(S);
   inc=0;
  //init dataset
  for (int i=0; i<N; i++) {

    //init coordinates
    data[i].x = (short)10+inc;//cmwc_rand();
    data[i].y = (short)20+inc;//cmwc_rand();
    inc= inc+10; 
   // printf("x:%d y:%d\n",data[i].x,data[i].y);
    //init label
    data[i].label = (unsigned char) (cmwc_rand()%C);
  }

/*#ifdef DEBUG
  printf("\n\n\nDATASET\n");
  printf("Idx \tX \tY \tLabel\n");
  for (int i=0; i<N; i++)
    printf("%d \t%d \t%d \t%d\n", i, data[i].x,  data[i].y, data[i].label);
#endif*/
  inc=0;
  //init test points
  for (int k=0; k<M; k++) {
    x[k].x  = (short)10;//cmwc_rand();
    x[k].y  = (short)15;//cmwc_rand();
     inc= inc+1; 
    //x[k].label will be calculated by the algorithm
  }

//#ifdef DEBUG
//  printf("\n\nTEST POINTS\n");
//  printf("Idx \tX \tY\n");
 // for (int k=0; k<M; k++)
  //  printf("%d \t%d \t%d\n", k, x[k].x, x[k].y);
//#endif
/************************************ NOT REQUIRED NOW, SINCE NEIGHBOURS ARE NOW DECIDED IN FPGA********/
    //init all k neighbors infinite distance
    //for (int j=0; j<K; j++)
      //neighbor[j].dist = INFINITE;

/************************************ START TIMER HERE ********************************/      
 // PROCESS DATA

/************************************************* INITIATING KNN H/W GETTING IT BASE ADDRESS AND SENDING SOFT RESET***
*************/
 elapsedua= timer_time_usec();
  knn_init( KNN_BASE);
// WRITE DATA TO FPGA ONLY ONCE FOR ALL TEST POINTS . NOT USED NOW COMMENTED OUT//////////////
  
 // data_wrd = 0;
  /*for(int i=0;i<N;i++)
  {
   data_wrd = 0;
   data_wrd = data[i].x;
  // printf("val1:%d\n",data_wrd);
   data_wrd <<=16;
  // printf("val2:%d\n",data_wrd);
   data_wrd |= data[i].y;
  // printf("val3:%d\n",data_wrd);
  knn_validin_set();
   IO_SET(base,KNN_DATA_PT,data_wrd);
    knn_validin_reset();
   
     }*/
/********************************WRITING DATA POINTS IN ARRAY. EACH ELEMENT OF ARRAY HAS 32 BIT FIRST [31:16]
    ARE FOR X CO-ORDINATE AND 15:0 ARE FOR Y CO-ORDINATE*******************/ 
   for(int i=0;i<N;i++)
  {
   data_wrd[i] = data[i].x;
   data_wrd[i] <<=16;
   data_wrd[i] |= data[i].y;
   }
/**************************WRITING DATA POINTS AND VALID IN SIGNAL TO FPGA***************/  
   IO_SET(base,KNN_DATA_PT1,data_wrd[0]);
   IO_SET(base,KNN_DATA_PT2,data_wrd[1]);
   IO_SET(base,KNN_DATA_PT3,data_wrd[2]);
   IO_SET(base,KNN_DATA_PT4,data_wrd[3]); 
   IO_SET(base,KNN_DATA_PT5,data_wrd[4]);
   IO_SET(base,KNN_DATA_PT6,data_wrd[5]);
   IO_SET(base,KNN_DATA_PT7,data_wrd[6]);
   IO_SET(base,KNN_DATA_PT8,data_wrd[7]); 

   IO_SET(base,KNN_VALID_IN,1);
   
  for (int k=0; k<M; k++) { 
	  //for all test points
	  //compute distances to dataset points

	#ifdef DEBUG
	    printf("\n\nProcessing x[%d]:\n", k);
	#endif
	#ifdef DEBUG
	    printf("Datum \tX \tY \tLabel \tDistance\n");
	#endif
	/*************************WRITING TEST POINT TO FPGA***************/
	IO_SET(base,KNN_VALID_IN,0);
	data_wrt=0;
	data_wrt = x[k].x;
	data_wrt <<=16;
	data_wrt |= x[k].y;
	IO_SET(base,KNN_TEST_PT,data_wrt);
	/******************************WRITING START FSM SIGMNAL TO FPGA*****/
	IO_SET(base,KNN_START,1); 
	/******************************POLLING FOR VALID OUT SIGNAL*********/
	  valid=0;	
	   while(1)
	   {
	   	valid = (int)IO_GET(base,KNN_VALID_OUT);
	   	if(valid)
	          break;
	   }
	/******************READING VALUES OF NEIGHBOURS AND THEIR INDEXS ********/  
	knn_sample_set();
	neighbor[0].dist = knn_rd_kn1();
   	neighbor[1].dist = knn_rd_kn2();
   	neighbor[2].dist = knn_rd_kn3();
	neighbor[3].dist = knn_rd_kn4();
	neighbor[4].dist = knn_rd_kn5();
   	neighbor[5].dist = knn_rd_kn6();
   	   
   	neighbor[0].idx = knn_rd_in1();
   	neighbor[1].idx = knn_rd_in2();
   	neighbor[2].idx = knn_rd_in3();
	neighbor[3].idx = knn_rd_in4();
	neighbor[4].idx = knn_rd_in5();
   	neighbor[5].idx = knn_rd_in6();
        knn_sample_reset();
	knn_stop();
	/********************NOT USED HERE ACCELERATED IN H/W--> EUCLIDEAN DISTANCE CALCULATION AND SORTING***********  
	    for (int i=3; i<N; i++) { 
	    
	    //for all dataset points
	    //compute distance to x[k]
	    //  unsigned int d = sq_dist(x[k], data[i]);   
	    //insert in ordered list
	    //for (int j=0; j<K; j++)
	     //   if ( distance[i] < neighbor[j].dist ) 
	      //  {
	       //  insert( (struct neighbor){i,distance[i]}, j);
	        // break;
	        //} 

	#ifdef DEBUG
	  // dataset
	      printf("%d \t%d \t%d \t%d \t%u\n", i, data[i].x, data[i].y, data[i].label, (uint32_t)neighbor[i].dist);
	#endif

	    }*/
	      
	    //classify test point

	    //clear all votes
	    int votes[C] = {0};
	    int best_votation = 0;
	    int best_voted = 0;

	    //make neighbours vote
	    for (int j=0; j<K; j++) { //for all neighbors
	      if ( (++votes[data[neighbor[j].idx].label]) > best_votation ) {
		best_voted = data[neighbor[j].idx].label;
		best_votation = votes[best_voted];
	      }
    	}

	    x[k].label = best_voted;

	    votes_acc[best_voted]++;
	    
	#ifdef DEBUG
	    printf("\n\nNEIGHBORS of x[%d]=(%d, %d):\n", k, x[k].x, x[k].y);
	    printf("K \tIdx \tX \tY \tDist \t\tLabel\n");
	    for (int j=0; j<K; j++)
	      printf("%d \t%d \t%d \t%d \t%d \t%d\n", j+1, neighbor[j].idx, data[neighbor[j].idx].x,  data[neighbor[j].idx].y, 				neighbor[j].dist,  data[neighbor[j].idx].label);
	    
	    printf("\n\nCLASSIFICATION of x[%d]:\n", k);
	    printf("X \tY \tLabel\n");
	    printf("%d \t%d \t%d\n\n\n", x[k].x, x[k].y, x[k].label);

	#endif

  } //all test points classified

  //stop knn here
  //read current timer count, compute elapsed time
  /******************READING TIMER COUNT****************************/
  elapsedub = timer_time_usec();
  printf("\nExecution time: %llu us %llu us @%dMHz\n\n", elapsedua,elapsedub, FREQ/1000000);

  
  //print classification distribution to check for statistical bias
  for (int l=0; l<C; l++)
    printf("%d ", votes_acc[l]);
  printf("\n");
 return 0; 
}
/********************************************/
// functions definitions
void knn_reset()
{
  IO_SET(base, KNN_RESET, 1);
  IO_SET(base, KNN_RESET, 0);	
}

void knn_init( int base_address)
{
  base = base_address;
  knn_reset();
  knn_validin_reset();
}

void knn_validin_set()
{
 IO_SET(base,KNN_VALID_IN,1);
}

void knn_validin_reset()
{
 IO_SET(base,KNN_VALID_IN,0);
}

void knn_start()
{
 IO_SET(base,KNN_START,1);
 	
}

void knn_stop()
{
 IO_SET(base,KNN_START,0);	
}

 int   knn_validout ()
{
 return (int)IO_GET(base,KNN_VALID_OUT);	
}

uint32_t knn_rd_kn1()
{
  return (uint32_t) IO_GET(base,KNN_KN1); 
}

uint32_t knn_rd_kn2()
{
  return (uint32_t) IO_GET(base,KNN_KN2); 
}

uint32_t knn_rd_kn3()
{
  return (uint32_t) IO_GET(base,KNN_KN3); 
}

uint32_t knn_rd_kn4()
{
  return (uint32_t) IO_GET(base,KNN_KN4); 
}

uint32_t knn_rd_kn5()
{
  return (uint32_t) IO_GET(base,KNN_KN5); 
}

uint32_t knn_rd_kn6()
{
  return (uint32_t) IO_GET(base,KNN_KN6); 
}


uint32_t knn_rd_in1()
{
  return (uint32_t) IO_GET(base,KNN_IN1); 
}

uint32_t knn_rd_in2()
{
  return (uint32_t) IO_GET(base,KNN_IN2); 
}

uint32_t knn_rd_in3()
{
  return (uint32_t) IO_GET(base,KNN_IN3); 
}

uint32_t knn_rd_in4()
{
  return (uint32_t) IO_GET(base,KNN_IN4); 
}

uint32_t knn_rd_in5()
{
  return (uint32_t) IO_GET(base,KNN_IN5); 
}

uint32_t knn_rd_in6()
{
  return (uint32_t) IO_GET(base,KNN_IN6); 
}

void knn_sample_set()
{
 IO_SET(base,KNN_SAMPLE,1);	
}

void knn_sample_reset()
{
 IO_SET(base,KNN_SAMPLE,0);	
}


