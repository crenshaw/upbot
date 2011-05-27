#include "supervisor.h"


/************************************************
  WME_unitTest.c

  This file contains a unit test to check the
  functionality of the functions related to our
  transition to WMEs as Episode state.

Auther: Zachary Paul Faltersack
Last Edit: May 27, 2011

To test the different WME value types, an argument
can be passed. This is an integer in the range 0..3
Default is 0 (integer)
0: integer
1: char
2: double
3: string

 ************************************************/

int main(int argc, char* argv[])
{
	// TESTING WME FUNCTIONS NOW WITH ARBITRARY DATA
	
	printf("\n================ TESTING BASIC WME FUNCTIONS NOW =================\n\n"); 
	printf("Started...\n");
	Vector* wmeVec1 = newVector();
	Vector* wmeVec2 = newVector();

	printf("Vectors 1 and 2 created...\n");
	printf("\nPopulating WME Vector 1\n");

	WME* wme;
	int i, type = (argc > 1 ? atoi(argv[1]) : 0);
	// Fill vector 1
	for(i = 0; i < 10; i++)
	{
		printf("Creating wme %d ... ", i);
		wme = (WME*)malloc(sizeof(WME));
		switch(type)
		{
			case WME_INT:
				wme->type = WME_INT;
				wme->value.iVal = i;
				break;
			case WME_CHAR:
				wme->type = WME_CHAR;
				sprintf(&(wme->value.cVal), "%d", i);
				break;
			case WME_DOUBLE:
				wme->type = WME_DOUBLE;
				wme->value.dVal = i + 0.00;
				break;
			case WME_STRING:
				wme->type = WME_STRING;
				wme->value.sVal = (char*)malloc(sizeof(char) * 10);
				strcpy(wme->value.sVal, "testStr");
				break;
		}//switch

		wme->attr = (char*)malloc(sizeof(char) * 2);
		sprintf(wme->attr, "%d", i);
		printf("Inserting wme %d into vector 1: ", i);
		addEntry(wmeVec1, wme);
		displayWME(wme);
		printf("\n");
	}//for

	printf("\nPopulating WME Vector 2\n");

	// Fill vector 2
	for(i = 0; i < 10; i++)
	{
		printf("Creating wme %d ... ", i);
		wme = (WME*)malloc(sizeof(WME));
		switch(type)
		{
			case WME_INT:
				wme->type = WME_INT;
				wme->value.iVal = i;
				break;
			case WME_CHAR:
				wme->type = WME_CHAR;
				sprintf(&(wme->value.cVal), "%d", i);
				break;
			case WME_DOUBLE:
				wme->type = WME_DOUBLE;
				wme->value.dVal = i + 0.00;
				break;
			case WME_STRING:
				wme->type = WME_STRING;
				wme->value.sVal = (char*)malloc(sizeof(char) * 10);
				strcpy(wme->value.sVal, "testStr");
				break;
		}//switch

		wme->attr = (char*)malloc(sizeof(char) * 2);
		sprintf(wme->attr, "%d", i);
		printf("Inserting wme %d into vector 2: ", i);
		addEntry(wmeVec2, wme);
		displayWME(wme);
		printf("\n");
	}//for

	printf("\nPrinting WME Vector 1\n");

	// Print from Vector 1
	for(i = 0; i < 10; i++)
	{
		printf("Vector 1 - Displaying wme %d: ", i);
		displayWME((WME*)getEntry(wmeVec1, i));
		printf("\n");
	}//for

	printf("\nPrinting WME Vector 2\n");

	// Print from Vector 2
	for(i = 0; i < 10; i++)
	{
		printf("Vector 2 - Displaying wme %d: ", i);
		displayWME((WME*)getEntry(wmeVec2, i));
		printf("\n");
	}//for

	printf("\nCreating EpisodeWME 1\n");
	EpisodeWME* ep1 = createEpisodeWME(wmeVec1);

	printf("\nCreating EpisodeWME 2\n");
	EpisodeWME* ep2 = createEpisodeWME(wmeVec2);

	printf("\nDisplaying EpisodeWME 1\n");
	displayEpisodeWME(ep1);

	printf("\nDisplaying EpisodeWME 2\n");
	displayEpisodeWME(ep2);

	printf("\nComparing EpisodeWMEs 1 and 2: Should be true\n");
	if(compareEpisodesWME(ep1, ep2, FALSE)) printf("Episodes are equal.\n");
	else printf("Episodes are not equal\n");

	printf("\nReversing values in Episode 2\n");

	// Reverse values in Episode 2
	for(i = 0; i < 10; i++)
	{
		wme = (WME*)getEntry(ep2->sensors, i);
		switch(wme->type)
		{
			case WME_INT:
				wme->value.iVal = 9 - i;
				break;
			case WME_CHAR:
				sprintf(&(wme->value.cVal), "%d", 9 - i);
				break;
			case WME_DOUBLE:
				wme->value.dVal = 9.00 - i;
				break;
			case WME_STRING:
				wme->value.sVal = (char*)malloc(sizeof(char) * 10);
				strcpy(wme->value.sVal, "strTest");
				break;
		}//switch
	}//for

	printf("\nDisplaying EpisodeWME 1\n");
	displayEpisodeWME(ep1);

	printf("\nDisplaying EpisodeWME 2\n");
	displayEpisodeWME(ep2);

	printf("\nComparing EpisodeWMEs 1 and 2: Should be false\n");
	if(compareEpisodesWME(ep1, ep2, FALSE)) printf("Episodes are equal.\n");
	else printf("Episodes are not equal\n");
	
	printf("\nFreeing memory\n");

	freeEpisodeWME(ep1);
	freeEpisodeWME(ep2);
	
	// BEGIN TEST FOR ROOMBA => WME CONVERSION

	// Test sensor strings taken from virtual world:
	char sense1[24] = "0000000000     1387   0";
	char sense2[24] = "0000000011     1392   1";

	printf("\n================ TESTING ROOMBA SENSOR CONVERSION NOW =================\n\n");
	printf("Sense string 1: \"%s\" \n", sense1);
	printf("Sense string 2: \"%s\" \n", sense2);

	printf("\nConverting senses to WME vectors\n");
	Vector* sense1Vec = roombaSensorsToWME(sense1);
	Vector* sense2Vec = roombaSensorsToWME(sense2);
	
	printf("\nPrinting Sense String 1 as WME Vector 1\n");

	// Print from Vector 1
	for(i = 0; i < 10; i++)
	{
		printf("Vector 1 - Displaying wme %d: ", i);
		displayWME((WME*)getEntry(sense1Vec, i));
		printf("\n");
	}//for

	printf("\nPrinting Sense String 2 as WME Vector 2\n");

	// Print from Vector 2
	for(i = 0; i < 10; i++)
	{
		printf("Vector 2 - Displaying wme %d: ", i);
		displayWME((WME*)getEntry(sense2Vec, i));
		printf("\n");
	}//for

	printf("\nCreating Episodes 1 and 2 with converted senses\n");
	ep1 = createEpisodeWME(sense1Vec);
	ep2 = createEpisodeWME(sense2Vec);

	printf("\nDisplaying EpisodeWME 1\n");
	displayEpisodeWME(ep1);

	printf("\nDisplaying EpisodeWME 2\n");
	displayEpisodeWME(ep2);
	
	printf("\nComparing EpisodeWMEs 1 and 2: Should be false\n");
	if(compareEpisodesWME(ep1, ep2, FALSE)) printf("Episodes are equal.\n");
	else printf("Episodes are not equal\n");

	printf("\nClearing EpisodeWME 2 and recreating with same sensor string as EpisodeWME 1\n");
	freeEpisodeWME(ep2);
	ep2 = createEpisodeWME(roombaSensorsToWME(sense1));
	
	printf("\nDisplaying EpisodeWME 1\n");
	displayEpisodeWME(ep1);

	printf("\nDisplaying EpisodeWME 2\n");
	displayEpisodeWME(ep2);
	
	printf("\nComparing EpisodeWMEs 1 and 2: Should be true\n");
	if(compareEpisodesWME(ep1, ep2, FALSE)) printf("Episodes are equal.\n");
	else printf("Episodes are not equal\n");

	printf("\nFreeing memory\n");

	freeEpisodeWME(ep1);
	freeEpisodeWME(ep2);

	printf("\nUnit Test Complete.\n");

	return 0;
}//main
