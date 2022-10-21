#define _CRT_SECURE_NO_WARNINGS


#include "Application.h"


int main()
{
	appstart();
}

void appstart(void)
{
	char scase = 0;

	do
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
		//////////////////////////////////////starting App/////////////////////////////////
		char again = 0;
		int elements = 0;
		float number = 0;
		uint8_t string = 0;
		scase = 0;
		printf("\n\n	*New Transaction* 'N'  ");
		printf("\n	*Set Max Transaction (Default 5000 EGP)* 'S'");
		printf("\n	*View the AccountDB* 'V'");
		printf("\n	*Generate the AccountDB* 'G'");
		printf("\n	*View Transaction DB list* 'L'");
		printf("\n	*Exit* 'E'\n	");
		scase = get_char();
		if (scase == 'N' || scase == 'n') //The APP *New Transaction* 'N' 
		{
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
							if (setMaxAmount(TermData) == TERMINAL_OK)
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

												printf("\n	Transaction Amount: APPROVED");
												//Printing The Transaction

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

											//////////////////////////////////END-SERVER/////////////////////////
										}
										else
										{
											printf("\n Declined Amount Exceeding Limit");
										}
										
									}
									else
									{
										printf("\n	**(Terminal)** Declined Expired Card");
									}

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


		else if (scase == 'S' || scase == 's')//*Set Max Transaction (Default 5000 EGP)* 'S'
		{
			if (setMaxAmount(TermData) != TERMINAL_OK)
			{
				printf("\n **Invalid Max**");
			}
		}
		else if (scase == 'V' || scase == 'v')//*View the AccountDB*
		{
			/* Getting the size of the file. */
			FILE* fp = fopen("Accounts DB.txt", "rb");
			fseek(fp, 0L, SEEK_END);
			int size = ftell(fp) / (sizeof(ST_accountsDB_t) + 27);
			fseek(fp, 0L, SEEK_SET);
			//printing the databatse
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
				if (buffer->state == 0)
					printf(" %0.2f / %s /Running\n", buffer->balance, buffer->primaryAccountNumber);
				else
					printf(" %0.2f / %s / Blocked\n", buffer->balance, buffer->primaryAccountNumber);
				free(buffer);	/* Freeing the memory allocated to the buffer */
			}

			fclose(fp);

		}
		else if (scase == 'G' || scase == 'g')
		{
			printf("\n	Enter the number of users: ");
			scanf("%i", &elements);
			getchar();// to get the feed line '\n' from scanf
			generateDB(elements);

		}
		else if (scase == 'L' || scase == 'l')
		{
			listSavedTransactions();
		}


	} while ((scase != 'E' && scase != 'e'));

	///////////////////////////CARD MODULE//////////////////////////////



}



