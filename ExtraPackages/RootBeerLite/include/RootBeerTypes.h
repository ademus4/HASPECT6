#ifndef ROOTBEER_TYPES
#define ROOTBEER_TYPES

#define SINGLE_BANK		0     	//BANK type where only one in an event
#define MULTIPLE_BANK		1	//BANK type where may be multiple (sectors) in an event
#define MAX_BANK_NUMBER		18	// max no of instances of same bank in one event 
#define MAX_BANK_TYPES	       	220	// no of different bank names allowed in clasbanks.ddl  	

// The files bankvars.cxx and bankvars.h define global varables, and a  print function etc
// for each BANK type defined in clasbanks.ddl
// bankvars.cxx and bankvars.h are generated by an awk script (bank2struct.gk),
// which parses the clasbanks.ddl file
//
// The global variables are (for example):
// 1) Single instance bank (eg EVNT):
//      struct EVNT_t *EVNT             //1d array of structures
//      int            EVNT_NH;         //no of hits in the bank
//
//
// 2) Multiple instance bank (eg DC0):
//      struct SCR_t    **SCR;          //2d array of structs
//      int              *SCR_NH;       //array of hits for each sector
//      int              *SCR_S;        //aray of sector numbers
//      int               SCR_NS;       //number of sectors
//
// The structure defined below holds the addresses of these global 
// variables for the BANK, plus some extra information
// including a print function

typedef struct addressBanks_t{
  char name[8]; 	//name of the bank
  void **address;	//address of array of structs (eg EVNT_t) for the bank
  int *nBank;		//address of variable with the no of sectors  / instances of BANK in the event 
  int **nRow;		//address of the array if hit nos for each sector which was present
  int **bankSec; 	//address if array of sector numbers for sectors which were present
  int length;		//length of the struct relating to the bank
  void (*pfunc)();	//-> print function for the bank
} addressBanks;

// bankvars.cxx and bankvars.h set up an array of these structs, with a member for each
// BANK, containing the addresses of the gobals etc.

#endif