#define _CRT_SECURE_NO_WARNINGS

#include "Server.h"

long File_Position = 0;

///////////////////////////////////isValidAccount/////////////////////////////////////////////
EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence == NULL)
	{
		printf("\n	ACCOUNT NOT FOUND");
		return ACCOUNT_NOT_FOUND;
	}
	else if (strcmp(cardData->primaryAccountNumber, accountRefrence->primaryAccountNumber) == 0)
	{
		return SERVER_OK;

	}
	else
	{
		printf("INTERAL ERROR");
		return ACCOUNT_NOT_FOUND;
	}
}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////isBlockedAccount////////////////////////////////////////
EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence->state == RUNNING)
	{
		return SERVER_OK;
	}
	else if (accountRefrence->state == BLOCKED)
	{
		printf("\n	This Acount is Bloucked");
		return BLOCKED_ACCOUNT;

	}

}
// ////////////////////Search_DB////////////////////////////////////////////////////////
// ///////////////////////isAmountAvailable//////////////////////////////////////
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence->balance >= termData->transAmount)
	{
		return SERVER_OK;
	}
	else
	{
		printf("\n	Not Enough Balance");
		return LOW_BALANCE;

	}

}
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////recieveTransactionData/////////////////////////////
EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	ST_accountsDB_t* Account = Search_DB(&transData->cardHolderData);
	char sure = '\0';
	uint8_t array[26]; //human readable content
	if (isValidAccount(&transData->cardHolderData, Account) == SERVER_OK)
	{
		float NewBalance = Account->balance - transData->terminalData.transAmount;
		if (isBlockedAccount(Account) == SERVER_OK)
		{
			if (isAmountAvailable(&transData->terminalData, Account) == SERVER_OK)
			{

				printf("\n	Current Balance: %0.2f\n", Account->balance);
				printf("\n	Confirm The request? (y/n): ");
				sure = get_char();

				if (sure == 'y' || sure == 'Y')
				{
					FILE* fp = fopen("Accounts DB.txt", "r+b");
					if (fp == NULL)
					{
						printf("\n	Open Database Failed");
						return INTERNAL_SERVER_ERROR;
					}
					printf("\n\n	###########################");
					printf("\n	Old Balance: %0.2f", Account->balance);
					printf("\n	Transaction Amount: %0.2f", transData->terminalData.transAmount);
					printf("\n	New Balance: %0.2f", NewBalance);
					printf("\n	Transaction Sequence Number: %i", transData->transactionSequenceNumber);
					printf("\n	Transaction Date: %s", transData->terminalData.transactionDate);
					printf("\n	Terminal Max Amount: %0.2f", transData->terminalData.maxTransAmount);
					printf("\n	Cardholder Name: %s", transData->cardHolderData.cardHolderName);
					printf("\n	PAN: %s", transData->cardHolderData.primaryAccountNumber);
					printf("\n	Card Expiration Date: %s", transData->cardHolderData.cardExpirationDate);
					printf("\n	###########################");
					
					Account->balance = NewBalance;
					if (Account->state == 0)
					{
						sprintf(array, "%/ %0.2f / RUNNING /",NewBalance); //writtin human readable content
					}
					else
					{
						sprintf(array, "%/ %0.2f / BLOCKED /", NewBalance); //writtin human readable content
					}
					fseek(fp, File_Position, SEEK_SET);
					fwrite(Account, sizeof(ST_accountsDB_t), 1, fp);
					fwrite(array, 26, 1, fp);
					fputc('\n', fp);// to get the feed line '\n'
					fclose(fp);
					free(Account);

					transData->transState = APPROVED;
					return APPROVED;
				}
				else
				{
					printf("\n (Server) Operation cancelled");
					return transData->transState = DECLINED_INSUFFECIENT_FUND;
					return DECLINED_INSUFFECIENT_FUND;
				}


			}
			else
			{
				free(Account);
				printf("\n	DECLINED INSUFFECIENT FUND");
				transData->transState = DECLINED_INSUFFECIENT_FUND;
				return DECLINED_INSUFFECIENT_FUND;
			}
		}
		else
		{
			free(Account);
			printf("\n	DECLINED STOLEN CARD");
			transData->transState = DECLINED_STOLEN_CARD;
			return DECLINED_STOLEN_CARD;
		}
	}
	else
	{
		free(Account);
		printf("\n	FRAUD CARD");
		transData->transState = FRAUD_CARD;
		return FRAUD_CARD;
	}

}
//Search_DB() searches the database for a matching PANand returns a pointer to the matching record
ST_accountsDB_t* Search_DB(ST_cardData_t* cardData)
{
	/*array of pointers points to structure*/


	/* Getting the size of the file. */

	FILE* fp = fopen("Accounts DB.txt", "rb");
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp) / (sizeof(ST_accountsDB_t) + 27);
	fseek(fp, 0L, SEEK_SET);



	for (int i = 0; i < size; i++)
	{
		ST_accountsDB_t* buffer = calloc(1, sizeof(ST_accountsDB_t));
		if (buffer == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return NULL;
		}
		fread(buffer, sizeof(ST_accountsDB_t), 1, fp);
		fseek(fp, 26, SEEK_CUR);
		fgetc(fp); // to get the feed line '\n'
		if (strcmp(cardData->primaryAccountNumber, buffer->primaryAccountNumber) == 0) //found tha PAN
		{
			File_Position = ftell(fp) - sizeof(ST_accountsDB_t) - 27;
			//printf("\nfound",File_Position);
			fclose(fp);
			return buffer;
		}
		free(buffer);
	}
	fclose(fp);
	return NULL; //return NULL if not found
}
///////////////////////////////////////
void generateDB(int elements) //generate random database
{
	time_t t;
	srand((unsigned)time(&t));
	FILE* fp = fopen("Accounts DB.txt", "wb");
	uint8_t array[26]; //human readable content 
	for (int i = 0; i < elements; i++)
	{
		srand((unsigned)time(&t));
		//file pointer fp point to the database
		ST_accountsDB_t* user1 = calloc(1, sizeof(ST_accountsDB_t));
		if (user1 == NULL)
		{
			printf("\n	NOT ENOUGH MEMORY");
			return;
		}
		//
		//	
		uint8_t* ptr = CardGenerator();

		printf("\n  Genetrated Luhn(Random Valid Credit card number): %s\n  You could use if you need it.\n", ptr);
		int r;
		if (rand() % 10 > 6)
		{
			r = 1;
		}
		else
		{
			r = 0;
		}

		user1->balance = (unsigned)(rand() % (100000 * !r + r * 1000)) + ((rand() % 100) / 100.0);

		strcpy(user1->primaryAccountNumber, ptr);

		user1->state = r;
		printf("%f   %s  %i ", user1->balance, user1->primaryAccountNumber, user1->state);
		fwrite(user1, sizeof(ST_accountsDB_t), 1, fp);
		if (r == 0)
		{
			sprintf(array, "%/ %0.2f / RUNNING /", user1->balance); //writtin human readable content
		}
		else
		{
			sprintf(array, "%/ %0.2f / BLOCKED /", user1->balance); //writtin human readable content
		}

		fwrite(array, 26, 1, fp);
		fputc('\n', fp);// to get the feed line '\n'
		fclose(fp);
		free(user1);
		t = time(&t);
		fp = fopen("Accounts DB.txt", "ab");
		while (t + 1 != time(&t)); //wait 1 sec
	}
}

/////////////////////////////////////////////////////////////////////////
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	FILE* fp = fopen("Transactions DB.txt", "a");
	if (fp == NULL)
	{
		printf("\n Failed Saving Transaction");
		return SAVING_FAILED;
	}

	//getting number of saved Transactions
	fseek(fp, 0, SEEK_END);
	long number = ftell(fp) / sizeof(ST_transaction_t);
	fseek(fp, 0, SEEK_SET);
	transData->transactionSequenceNumber = number;
	if (fp == NULL)
	{
		printf("\n	(Server)Transactions Database Error");
		return SAVING_FAILED;
	}
	fwrite(transData, sizeof(ST_transaction_t), 1, fp);
	fputc('\n', fp);
	fclose(fp);
	return SERVER_OK;
}

/////////////////////////////////////////////////////////////
void listSavedTransactions(void)
{
	ST_transaction_t buffer;
	FILE* fp = fopen("Transactions DB.txt", "r");
	if (fp == NULL)
	{
		printf("\n Failed Open The Transactions Database");
		return;
	}
	//getting number of saved Transactions
	fseek(fp, 0, SEEK_END);
	int number = ftell(fp) / sizeof(ST_transaction_t);
	fseek(fp, 0, SEEK_SET);


	printf("\n\n	/////////////////////////////////Saved Transactions///////////////////////\n");
	for (int i = 0; i < number; i++)
	{
		fread(&buffer, sizeof(ST_transaction_t), 1, fp);
		fgetc(fp); // gets the \n feed line
		printf("\n	Transaction Sequence Number: %i", buffer.transactionSequenceNumber);
		printf("\n	Transaction Date:: %s", buffer.terminalData.transactionDate);
		printf("\n	Transaction Amount: %0.2f", buffer.terminalData.transAmount);
		if (buffer.transState == APPROVED)
		{
			printf("\n	Transaction : APPROVED");
		}
		else if (buffer.transState == DECLINED_INSUFFECIENT_FUND)
		{
			printf("\n	Transaction : DECLINED INSUFFECIENT FUND");
		}
		else if (buffer.transState == DECLINED_STOLEN_CARD)
		{
			printf("\n	Transaction : DECLINED STOLEN CARD");
		}
		else if (buffer.transState == FRAUD_CARD)
		{
			printf("\n	Transaction : FRAUD CARD");
		}
		else if (buffer.transState == INTERNAL_SERVER_ERROR)
		{
			printf("\n	Transaction Amount: INTERNAL SERVER ERROR");
		}
		printf("\n	Terminal Max Amount: %0.2f", buffer.terminalData.maxTransAmount);
		printf("\n	Cardholder Name: %s", buffer.cardHolderData.cardHolderName);
		printf("\n	PAN: %s", buffer.cardHolderData.primaryAccountNumber);
		printf("\n	Card Expiration Date: %s", buffer.cardHolderData.cardExpirationDate);
		printf("\n	###########################");
	}

	fclose(fp);

}
////////////////////////////////////////////////
void isValidAccountTest(void)
{
	while (1)
	{
		ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));
		if (getCardPAN(CardData) == CARD_OK)
		{
			if (isValidAccount(CardData, Search_DB(CardData)) == SERVER_OK)
			{
				printf("\n	Account exists in the database");
			}
			else
			{
				printf("\n	Account Not found in the database");
			}
		}

		else
		{
			printf("\n	Please enter valid PAN");
		}
	}
}
////////////////////////////////////////////////
void isBlockedAccountTest(void)
{
	while (1)
	{
		ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));
		if (getCardPAN(CardData) == CARD_OK)
			if (isValidAccount(CardData, Search_DB(CardData)) == SERVER_OK)
			{
				printf("\n	Account exists ");
				if (isBlockedAccount(Search_DB(CardData)) == SERVER_OK)
					printf("\n Account is RUNNING");
				else
					printf("\n Account is BLOCKED");
			}
			else
			{
				printf("\n	Account Not found ");
			}
		else
		{
			printf("\n	Please enter valid PAN");
		}
		free(CardData);
	}
}
////////////////////////////////////////////////////////////
void isAmountAvailableTest(void)
{
	while (1)
	{
		ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));

		if (getCardPAN(CardData) == CARD_OK)
		{
			ST_accountsDB_t* Account = Search_DB(CardData);
			if (isValidAccount(CardData, Account) == SERVER_OK)
			{
				printf("\n	Account exists ");
				if (isBlockedAccount(Account) == SERVER_OK)
				{
					printf("\n Account is RUNNING");
					printf("\n	Account balance: %0.2f", Account->balance);

					ST_terminalData_t* TermData = calloc(1, sizeof(ST_terminalData_t));
					if (getTransactionAmount(TermData) == TERMINAL_OK)
					{
						if (isAmountAvailable(TermData, Account) == SERVER_OK)
						{
							printf("\n The Amount Is avaible");
						}
						else
						{
							printf("\n The Amount not avaible");
						}
					}


				}

				else
					printf("\n Account is BLOCKED");
			}
			else
			{
				printf("\n	Account Not found ");
			}
		}
		else
		{
			printf("\n	Please enter valid PAN");
		}
		free(CardData);
		printf("\n ####################################");
	}

}
////////////////////////////////////////////////////////////
void saveTransactionTest(void)
{
	printf("\n	     *****It's Better to get PAN from the AccountDB.txt To Make Sure that's Valid Account***** \n\n");
	ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));

	ST_terminalData_t* TermData = calloc(1, sizeof(ST_terminalData_t));

	ST_transaction_t* Data2Server = calloc(1, sizeof(ST_transaction_t));

	///////////////////////////CARD MODULE//////////////////////////////
	if (getCardHolderName(CardData) == CARD_OK)	// Getting Card Name from User
	{
		printf("\n	***Valid name***\n");
		if (getCardExpiryDate(CardData) == CARD_OK) //EXP date
		{
			printf("\n	***Valid Format***\n");
			uint8_t* ptr = CardGenerator(); // Generate Random Valid Luhn Number

			printf("\n  Genetrated Luhn(Random Valid Credit card number): %s\n  You could use if you need it.\n", ptr);
			free((char*)ptr); // free The string of that number
			printf("\n	It's Better to get The PAN from Database  ");

			if (getCardPAN(CardData) == CARD_OK)	//Gettin PAN
			{
				printf("\n\n	/////////////////////////TERMINAL MODULE///////////////////////////\n");
				/////////////////////////TERMINAL MODULE///////////////////////////
				if (getTransactionAmount(TermData) == TERMINAL_OK) //check if the amount is below the max
				{

					getTransactionDate(TermData); /*it Automatically gets the date */

					if (isValidCardPAN(CardData) == TERMINAL_OK)
					{
						if (isCardExpired(*CardData, *TermData) == TERMINAL_OK) //Is Card Expired ?
						{
							printf("\n	***Valid Card***\n");
							if (isBelowMaxAmount(TermData) == TERMINAL_OK) //Is Excedded Amount ?
							{

								printf("\n	-->Connecting to the Server...\n");
								printf("\n\n	/////////////////////////////////SERVER-MODULE///////////////////////\n");
								/////////////////////////////////SERVER-MODULE///////////////////////
								Data2Server->terminalData = *TermData;
								Data2Server->cardHolderData = *CardData;
								int saveTrans = recieveTransactionData(Data2Server); //avoiding calling the func many times
								/////////////// Saving Transaction data
								if (saveTransaction(Data2Server) == SERVER_OK)
								{
									printf("\n	Saved Transaction");
								}
								else if (saveTrans == SAVING_FAILED)
								{
									printf("\n	INTERNAL SERVER ERROR");
									Data2Server->transState = SAVING_FAILED;
									return SAVING_FAILED;
								}
								if (saveTrans == APPROVED)
								{
									ST_accountsDB_t* Account = Search_DB(CardData);
									float NewBalance = Account->balance - Data2Server->terminalData.transAmount;
									printf("\n\n	Check Database\n\n");
									//Printing The Transaction
									printf("\n\n	###########################");
									printf("\n	Old Balance: %0.2f", Account->balance);
									printf("\n	Transaction Amount: %0.2f", Data2Server->terminalData.transAmount);
									printf("\n	New Balance: %0.2f", NewBalance);
									printf("\n	Transaction Sequence Number: %i", Data2Server->transactionSequenceNumber);
									printf("\n	Transaction Date: %s", Data2Server->terminalData.transactionDate);
									printf("\n	Terminal Max Amount: %0.2f", Data2Server->terminalData.maxTransAmount);
									printf("\n	Cardholder Name: %s", Data2Server->cardHolderData.cardHolderName);
									printf("\n	PAN: %s", Data2Server->cardHolderData.primaryAccountNumber);
									printf("\n	Card Expiration Date: %s", Data2Server->cardHolderData.cardExpirationDate);
									printf("\n	###########################");
								}
								else if (saveTrans == DECLINED_INSUFFECIENT_FUND)
								{
									printf("\n	DECLINED INSUFFECIENT FUND");
								}
								else if (saveTrans == DECLINED_STOLEN_CARD)
								{
									printf("\n	 DECLINED STOLEN CARD");
								}
								else if (saveTrans == FRAUD_CARD)
								{
									printf("\n	FRAUD CARD");
								}
								else if (saveTrans == INTERNAL_SERVER_ERROR)
								{
									printf("\n	 INTERNAL SERVER ERROR");
								}


							}
							else
							{
								printf("\n Declined Amount Exceeding Limit");
							}

							//////////////////////////////////END-SERVER/////////////////////////
						}
						else
						{
							printf("\n	**(Terminal)** Declined Expired Card");
						}

					}

					////////////////////////////End-Terminal/////////////////////////////


				}
				

			}
			else
			{
				printf("\n	Entered Wrong PAN");
			}
		}
		else
		{
			printf("\n	InValid Format");
		}

	}
	else
	{
		printf("\n	Entered Wrong Name");
	}
	free(CardData);
	free(TermData);
	free(Data2Server);
	////////////////////////////END-CARD/////////////////////////////////////////
}

void recieveTransactionDataTest(void)
{
	printf("\n	     *****It's Better to get PAN from the AccountDB.txt To Make Sure that's Valid Account***** \n\n");
	ST_cardData_t* CardData = calloc(1, sizeof(ST_cardData_t));
	if (CardData == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}

	ST_terminalData_t* TermData = calloc(1, sizeof(ST_terminalData_t));
	if (TermData == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}

	ST_transaction_t* Data2Server = calloc(1, sizeof(ST_transaction_t));
	if (Data2Server == NULL)
	{
		printf("\n	NOT ENOUGH MEMORY");
		return;
	}

	///////////////////////////CARD MODULE//////////////////////////////
	if (getCardHolderName(CardData) == CARD_OK)	// Getting Card Name from User
	{
		printf("\n	***Valid name***\n");
		if (getCardExpiryDate(CardData) == CARD_OK) //EXP date
		{
			printf("\n	***Valid Format***\n");
			uint8_t* ptr = CardGenerator(); // Generate Random Valid Luhn Number

			printf("\n  Genetrated Luhn(Random Valid Credit card number): %s\n  You could use if you need it.\n", ptr);
			free((char*)ptr); // free The string of that number
			printf("\n	It's Better to get The PAN from Database  ");
			while (1)
			{
				if (getCardPAN(CardData) == CARD_OK)	//Gettin PAN
				{
					printf("\n\n	/////////////////////////TERMINAL MODULE///////////////////////////\n");
					/////////////////////////TERMINAL MODULE///////////////////////////
					if (setMaxAmount(TermData) == TERMINAL_OK) //setting max amount
					{
						if (isBelowMaxAmount(TermData) == TERMINAL_OK) //check if the amount is below the max
						{

							getTransactionDate(TermData); /*it Automatically gets the date */

							if (isValidCardPAN(CardData) == TERMINAL_OK)
							{
								if (isCardExpired(*CardData, *TermData) == TERMINAL_OK) //Is Card Expired ?
								{
									printf("\n	***Valid Card***\n");
									if (getTransactionAmount(TermData) == TERMINAL_OK) //Is Excedded Amount ?
									{

										printf("\n	-->Connecting to the Server...\n");
										printf("\n\n	/////////////////////////////////SERVER-MODULE///////////////////////\n");
										/////////////////////////////////SERVER-MODULE///////////////////////
										Data2Server->terminalData = *TermData;
										Data2Server->cardHolderData = *CardData;
										int saveTrans = recieveTransactionData(Data2Server); //avoiding calling the func many times
										/////////////// Saving Transaction data
										if (saveTransaction(Data2Server) == SERVER_OK)
										{

										}
										else if (saveTrans == SAVING_FAILED)
										{
											printf("\n	INTERNAL SERVER ERROR");
											Data2Server->transState = SAVING_FAILED;
											return SAVING_FAILED;
										}
										printf("\n ###################");
										if (saveTrans == APPROVED)
										{
											ST_accountsDB_t* Account = Search_DB(CardData);
											float NewBalance = Account->balance - Data2Server->terminalData.transAmount;
											printf("\n	Transaction Amount: APPROVED");


										}
										else if (saveTrans == DECLINED_INSUFFECIENT_FUND)
										{
											printf("\n	DECLINED INSUFFECIENT FUND");
										}
										else if (saveTrans == DECLINED_STOLEN_CARD)
										{
											printf("\n	 DECLINED STOLEN CARD");
										}
										else if (saveTrans == FRAUD_CARD)
										{
											printf("\n	FRAUD CARD");
										}
										else if (saveTrans == INTERNAL_SERVER_ERROR)
										{
											printf("\n	 INTERNAL SERVER ERROR");
										}
										printf("\n ###################");


									}
									//////////////////////////////////END-SERVER/////////////////////////
								}
								else
								{
									printf("\n	**(Terminal)** Declined Expired Card");
								}
							}
							
							////////////////////////////End-Terminal/////////////////////////////


						}
						else
						{
							printf("\n Declined Amount Exceeding Limit");
						}
					}

				}
				else
				{
					printf("\n	Entered Wrong PAN");
				}
			}
		}
		else
		{
			printf("\n	InValid Format");
		}

	}
	else
	{
		printf("\n	Entered Wrong Name");
	}
	free(CardData);
	free(TermData);
	free(Data2Server);
	////////////////////////////END-CARD/////////////////////////////////////////
}
























