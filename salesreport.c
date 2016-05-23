//SalesReport - Spring 2016 Final
//Lew Piper III | Palomar ID 011263249

/*Include Statements*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct item_attributes{
	char* itemid;
	char* description;
	float price;
	
	int quantity_sold;
} item_attributes;

typedef struct sales_manifest{
	char* itemid;
} sales_manifest;

/*Function Prototypes and Global Variables*/
void processFile ( FILE* fpSalesSummaryWrite, item_attributes** item_rows_out, size_t* item_attributes_count_out, sales_manifest** sale_rows_out, size_t* sales_manifest_count_out);
void calculateSales ( item_attributes* item_rows_out, size_t item_attributes_count_out, sales_manifest* sale_rows_out, size_t sales_manifest_count_out);
void outputFile ( FILE* fpSalesSummaryWrite, item_attributes* item_rows, size_t item_attributes_count, sales_manifest* sale_rows, size_t sales_manifest_count);


/*Main Function*/
int main(int argc, char *argv[]) {
	FILE* fpSalesSummaryWrite = fopen("SalesSummary.txt", "w");
	
	item_attributes* item_rows;
	size_t item_attributes_count;
	sales_manifest* sale_rows;
	size_t sales_manifest_count;
	
	//Function calls
	processFile(fpSalesSummaryWrite, &item_rows, &item_attributes_count, &sale_rows, &sales_manifest_count);
	calculateSales(item_rows, item_attributes_count, sale_rows, sales_manifest_count);
	outputFile(fpSalesSummaryWrite, item_rows, item_attributes_count, sale_rows, sales_manifest_count);
	outputFile(stdout, item_rows, item_attributes_count, sale_rows, sales_manifest_count);
	
	//Closing the write
	fclose(fpSalesSummaryWrite);
	
	//Free memory no longer needed
	free(item_rows);
	free(sale_rows);
	return 0;
}

/*Function Opens and Reads Files*/
void processFile ( FILE *fpSalesSummaryWrite, item_attributes** item_rows_out, size_t* item_attributes_count_out
				 , sales_manifest** sale_rows_out, size_t* sales_manifest_count_out)
{
	FILE *fpItemListRead = NULL;
	FILE *fpItemSalesRead = NULL;
  
	//Open item list file for reading
	if(!(fpItemListRead = fopen("ItemList.txt", "r"))){
		printf("File %s could not be opened. \n", "ItemList.txt");
		fprintf(fpSalesSummaryWrite,"File %s could not be opened. \n", "ItemList.txt");
		exit(1);
	}
	
	//Open item sales file for reading
	if(!(fpItemSalesRead = fopen("ItemSales.txt", "r"))){
		printf("File %s could not be opened. \n", "ItemSales.txt");
		fprintf(fpSalesSummaryWrite,"File %s could not be opened. \n", "ItemSales.txt");
		exit(1);
	}

	//Variables to keep track of the count and memory needed for reading of info from files
	item_attributes* item_rows = malloc(sizeof(item_attributes)*10);
	size_t item_attributes_count = 0;
	size_t item_attributes_capacity = 10;
	sales_manifest* sale_rows = malloc(sizeof(sales_manifest)*10);
	size_t sales_manifest_count = 0;
	size_t sales_manifest_capacity = 10;

	while (!feof(fpItemListRead)) {
		//Temp storage for content read in
		char itemidbuf[512];
		char descbuf[512];
		float price;
		//Check for and resize the capacity of the array if necessary
		if (item_attributes_count == item_attributes_capacity) {
			item_attributes_capacity *= 2;
			item_rows = realloc(item_rows, sizeof(item_attributes) * item_attributes_capacity);
		}
		
		//Place attributes into the item rows array and increment the count
		item_attributes* item_row = &item_rows[item_attributes_count++];
	
		//Scan itemlist file and place contents into temp storage
		if (fscanf(fpItemListRead, " %s %s %f ", itemidbuf, descbuf, &price) == EOF)
			break;
		//Alocate string of the correct length, copy it from buffer for item and descript
		
		//Debugging was needed the orginal code is commented out below
		item_row->itemid = malloc(strlen(itemidbuf)+1);
		strcpy(item_row->itemid, itemidbuf);
		item_row->description = malloc(strlen(descbuf)+1);
		strcpy(item_row->description, descbuf);
		//Place price from buffer into pirce
		item_row->price = price;
		//Initialize quantiiy sold for each iteration of the loop
		item_row->quantity_sold = 0;
	}

	while (!feof(fpItemSalesRead)) {
		//Temp storage for content read in
		char itemidbuf[512];

		//Check for and resize the capacity of the array if necessary
		if (sales_manifest_count == sales_manifest_capacity) {
			///we need to resize the array
			sales_manifest_capacity *= 2;
			sale_rows = realloc(sale_rows, sizeof(sales_manifest) * sales_manifest_capacity);
		}
		
		//Place sales data into the sale rows array
		sales_manifest* sale_row = &sale_rows[sales_manifest_count++];
		
		//Scan the itemsales file and palce the content into temp storage
		if (fscanf(fpItemSalesRead, " %s ", itemidbuf) == EOF)
			break;
		
		//Allocate string of the correct length and copy it from temp storage 
		sale_row->itemid = malloc(strlen(itemidbuf)+1);
		strcpy(sale_row->itemid, itemidbuf);
	}

	//Error if there is no data in the ItemList.txt file
	if (item_attributes_count == 0) {
		printf("Invalid input file\n");
		exit(0);
	}
	//Error if there is no data in the ItemSales.txt file
	if (sales_manifest_count == 0) {
		printf("Invalid input file\n");
		exit(0);
	}
	
	//Close the files when done with them
	fclose(fpItemListRead);
	fclose(fpItemSalesRead);
	
	//Return values needed throughout the rest of the program
	*item_rows_out = item_rows;
	*item_attributes_count_out = item_attributes_count;
	*sale_rows_out = sale_rows;
	*sales_manifest_count_out = sales_manifest_count;
}

//Find which row the item is at in the itemlist file that was read in
item_attributes* find_item_row(const char* itemid, item_attributes* item_rows, size_t item_attributes_count) {
	for (size_t i = 0; i < item_attributes_count; ++i) {
		item_attributes* item_row = &item_rows[i];
		//Compare the strings to see if it matches and if it does return that row
		if (strcmp(item_row->itemid, itemid)==0) {
			return item_row;
		}   
	}
	//If an item is not found return NULL as an error
	return NULL;
}

///*Takes information read in from two input files and compares the itemid of both to calculate the quantity sold of each item*/
void calculateSales (item_attributes* item_rows, size_t item_attributes_count,sales_manifest* sale_rows, size_t sale_rows_count) {
	for (size_t i = 0; i < sale_rows_count; ++i) {
		//Get the row data and itemid from that row
		sales_manifest* sale_row = &sale_rows[i];
		const char* itemid = sale_row->itemid;
		//Call find_item_row function to locate compare salelist and itemlist itemid information
		item_attributes* item_row = find_item_row(itemid, item_rows, item_attributes_count);
		//Error handling if the item is not found in the itemlist
		if (item_row == NULL){
			printf("This item does not appear to be in the itemlist file as a valid item\n");
			exit(-1);
		}
		//Increment the quantity sold for that item at the row that was returned
		item_row->quantity_sold++;
	}
}

/*Output file with formatting*/
void outputFile (FILE* out, item_attributes* item_rows, size_t item_attributes_count, sales_manifest* sale_rows, size_t sales_manifest_count) {
	float all_total_sales = 0.0;
	
	fprintf(out,"%10s %20s %20s\n", "Item Number", "Item Description", "Quantity Sold");
	fprintf(out,"%10s %20s %20s\n", "-----------", "----------------", "-------------");
    //Loop for printing part one
	for (size_t i = 0; i < item_attributes_count; ++i) {
		item_attributes* item_row = &item_rows[i];
		fprintf(out,"%10s %20s %20d\n", item_row->itemid, item_row->description, item_row->quantity_sold);
	}
	
	fprintf(out,"\n\n\n");
	fprintf(out,"%10s %20s %20s %20s %20s\n", "Item Number", "Item Description", "Quantity Sold", "Unit Price", "Total Sales");
	fprintf(out,"%10s %20s %20s %20s %20s\n", "-----------", "----------------", "-------------", "----------", "-----------");
    //Loop for printing part two
	for (size_t i = 0; i < item_attributes_count; ++i) {
		item_attributes* item_row = &item_rows[i];
		float total_sales = item_row->quantity_sold * item_row->price;
		float all_total_sales = 0.0;
		all_total_sales += total_sales;
		fprintf(out,"%10s %20s %20d %20.2f %20.2f\n", item_row->itemid, item_row->description, item_row->quantity_sold, item_row->price, total_sales);
	}
	
	fprintf(out,"\n\n\n");
	//Loop for printing part three
	for(size_t i = 0; i < item_attributes_count; ++i) {
		item_attributes* item_row = &item_rows[i];
		float total_sales = item_row->quantity_sold * item_row->price;
		all_total_sales += total_sales;
	}
	fprintf(out,"Total Sales: %10.2f", all_total_sales);
}