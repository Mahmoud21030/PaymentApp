#define _CRT_SECURE_NO_WARNINGS

#include "Terminal.h"


//////////////////////////////////////getTransactionDate//////////////////////////
/**
 * It gets the current date and time and stores it in a string
 * termData->transactionDate is a pointer to a structure that contains the transaction data.
 */
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	time_t mytime = time(NULL);           // getting time in sec
	struct tm* date = localtime(&mytime); // getting time in structure
	// year100 to make sure that is only 2 digits
	sprintf(termData->transactionDate,
		"%02i/%02i/%04i", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);

	return TERMINAL_OK;
}
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////isCardExpired///////////////////////////////////////////
EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)

{                                                   /*Converting String to integers first cz it's easier */
	int month = atoi(termData.transactionDate + 3); // from this format dd/mm/yyyy month is the index 3
	int year = atoi(termData.transactionDate + 8);  /*from this format dd / mm / yyyy year is the index 8
	  cz we need only last 2 digits.*/

	  // same for card date

	int monthcard = atoi(cardData.cardExpirationDate);    // from this format mm/yy month is the index 0
	int yearcard = atoi(cardData.cardExpirationDate + 3); // from this format  mm / yy year is the index 3

	if (yearcard > year)
	{
		return TERMINAL_OK;
	}
	else if (yearcard == year)
	{
		if (monthcard >= month)
		{
			return TERMINAL_OK;
		}
		return EXPIRED_CARD;
	}
	return EXPIRED_CARD; //else
}

///////////////////////////////////////////////////////////////////
/////////////////////////getTransactionAmount/////////////////////
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float temp = 0;
	uint8_t	string[20];
	printf("\n    Enter Transaction Amount: ");
	gets(string);

	if (s_isfloat(string) == 1)
	{
		temp = atof(string);
		if (temp <= 0)
		{
			return INVALID_AMOUNT;
		}

		printf("%0.2f", temp);
	}
	else
	{
		return INVALID_AMOUNT;
	}
	termData->transAmount = temp;

	return TERMINAL_OK;
}

///////////////////////////////////////////////////////////////////
/////////////////////////setMaxAmount/////////////////////////////
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{ /* Setting the default value of the max amount to 5000. */
	char cond = 0;
	static float temp = 5000;
	uint8_t	string[20];

	printf("\n  Default Max is: %0.2f EGP",temp);
	printf("\n  Set Max Amount (y/n): ");
	/* Asking the user if they want to change the max amount. */
	cond = get_char();

	if (cond == 'y' || cond == 'Y')
	{
		printf("\n  Enter the Max amount:");
		gets(string);
		if (s_isfloat(string) == 1)
		{
			temp = atof(string);
			if (temp <= 0)
			{
				temp = 5000;
				return INVALID_AMOUNT;
			}
			termData->maxTransAmount = temp;
			printf("%0.2f", temp);
		}
		else
		{
			return INVALID_AMOUNT;
		}

	}
	termData->maxTransAmount = temp;
	printf("\n Terminal Max Amount: %0.2f", temp);
	return TERMINAL_OK;


}

///////////////////////////////////////////////////////////////////
/////////////////////////isBelowMaxAmount/////////////////////////
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	if (termData->transAmount > termData->maxTransAmount)
	{
		printf("\nDeclined Amount Exceeding Limit");
		return EXCEED_MAX_AMOUNT;
	}

	return TERMINAL_OK;
}

///////////////////////////////////////////////////////////////////
/////////////////////isValidCardPAN//////////////////////////////
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	if (check_luhn(cardData->primaryAccountNumber) == 0)
		/*this func implemeted before return the last digit of the Luhn sum
		if the last digit of luhn sum is not 0 this isn't valid card */
	{
		return TERMINAL_OK;
	}
	printf("\n	Declined Invalid Card PAN ");
	return INVALID_CARD;
}
/////////////////////////////////////////////////////////////



void getTransactionDateTest(void)
{
	char again = 'Y';
	//initializing termData struct
	ST_terminalData_t* termData = calloc(1, sizeof(ST_cardData_t));
	if (termData == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}
	while (again == 'Y' || again == 'y')
	{
		getTransactionDate(termData);
		printf("\n	%s", termData->transactionDate);
		printf("\n	Again? 'Y': ");
		again = get_char();
	}
	free(termData);

}
/////////////////////////////////////////////////////////
void isCardExpriedTest(void)
{
	char again = 'Y';
	//initializing termData struct
	ST_terminalData_t* termData = calloc(1, sizeof(ST_cardData_t));
	ST_cardData_t* cardData = calloc(1, sizeof(ST_cardData_t));
	if (cardData == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}
	if (termData == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}
	getTransactionDate(termData);
	while (again == 'Y' || again == 'y')
	{
		if (getCardExpiryDate(cardData) == CARD_OK) {
			if (isCardExpired(*cardData, *termData) == CARD_OK)
			{
				printf("\n	Card isn't Expired(Valid)");

			}
			else
			{
				printf("EXPIRED CARD");
			}
		}
		else
		{
			printf("\n	WRONG EXP DATE  \n");
		}
		printf("\n	Again? 'Y': ");
		again = get_char();

	}
	free(termData);
	free(cardData);

}
//////////////////////////////////////////////////
void getTransactionAmountTest(void)
{



	while (1)
	{
		ST_terminalData_t* termData = calloc(1, sizeof(ST_cardData_t));
		if (termData == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return;
		}
		if (getTransactionAmount(termData) == TERMINAL_OK)
		{
			printf("\n	VALID AMOUNT");

		}
		else
		{
			printf("\n	INVALID AMOUNT");
		}

		free(termData);

	}


}

void setMaxAmountTest(void)
{
	char again = 'Y'; //initializing termData struct
	do
	{
		ST_terminalData_t* termData = calloc(1, sizeof(ST_cardData_t));
		if (termData == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return;
		}

		if (setMaxAmount(termData) == TERMINAL_OK)
		{
			printf("\n	VALID MAX");
		}
		else
		{
			printf("\n	INVALID MAX");
		}
		printf("\n	Again? 'Y': ");
		again = get_char();

		free(termData);
		printf("\n###################");
	} while (again == 'Y' || again == 'y');

}
///////////////////////////////////////////


void isBelowMaxAmountTest(void)
{
	while (1)
	{
		ST_terminalData_t* termData = calloc(1, sizeof(ST_cardData_t));
		if (termData == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return;
		}
		if (setMaxAmount(termData) == TERMINAL_OK)
			if (getTransactionAmount(termData) == TERMINAL_OK)
				if (isBelowMaxAmount(termData) == TERMINAL_OK)
				{
					printf("\n	Valid Max >= Transaction amount");
				}
				else
				{
					printf("\nNot Valid");
				}
		printf("\n\n	##########################################\n");
		free(termData);
	}

}
void isValidCardPANTest(void)
{
	while (1)
	{
		uint8_t* number = CardGenerator();
		printf("\n	Random Generated Valid Card: %s", number);
		ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));
		if (CardData == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return;
		}
		if (getCardPAN(CardData) == TERMINAL_OK)
			if (isValidCardPAN(CardData) == TERMINAL_OK)
			{
				printf("\n	Valid PAN Luhn");
			}
			else
			{
				printf("\n	Invalid PAN luhn");
			}
		printf("\n	##########################\n");
		free(CardData);
	}
}
