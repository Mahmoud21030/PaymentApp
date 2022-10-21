#ifndef Card_h
#define Card_h

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>



typedef struct ST_cardData_t
{
	uint8_t cardHolderName[25];
	uint8_t primaryAccountNumber[20];
	uint8_t cardExpirationDate[6];
}ST_cardData_t;

typedef enum EN_cardError_t
{
	CARD_OK,
	WRONG_NAME,
	WRONG_EXP_DATE,
	WRONG_PAN
}EN_cardError_t;

EN_cardError_t getCardHolderName(ST_cardData_t* cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData);
EN_cardError_t getCardPAN(ST_cardData_t* cardData);

uint8_t check_luhn(char* string);
uint8_t* CardGenerator(void);

void getCardHolderNameTest(void);
void getCardExpiryDateTest(void);
void getCardPANTest(void);

bool s_isalpha(char* string);
bool s_isdigit(char* string);
bool s_isdate(char* string);
bool s_isfloat(uint8_t* string);
char get_char(void);

#endif 
