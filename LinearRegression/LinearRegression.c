/*LinearRegression:
This program will run by initiating which files the user wants the program to read(Example: ./LinearRegression textfile1.txt textfile2.txt).
The program will do the following:
    - Read each file individually whilst counting how many characters there are in the file and allocating/reallocating space to a pointer called temparrayinstring.
    - In the same cycle the program will aquire each character and allocate it to temparrayinstring.
    - StringToken will be used to remove unwanted characters such as ',' or empty values this is filtered and allocated to another pointer called ptr.
    - An arrayofint is created with the size 4 times larger than temparrayinstring to hold integers.
    - The contents on ptr will be converted to int and allocated to arrayofint.
    - Due to the array being 1 dimension, modules was used to aquire X and Y.
    - After getting X and Y, then LR was aquired.

 After running the program enter value of Y.

    To run code:
        gcc LinearRegression.c -o LinearRegression;
        ./LinearRegression datasetLR1.txt datasetLR2.txt datasetLR3.txt datasetLR4.txt; 
        rm LinearRegression;

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    This function is used to calculate the values of 'A' and 'B', which are used for linear regression. The variables 'A' and 'B' are pased in as pointers, as
    are the variables 'SumofX', 'SumofY', 'SumofXY' ,'SumofX2', 'SumofY2, and 'Inputs'.
    The values 'A' and 'B' are then stored in the memory location pointed to by the pointers 'A' and 'B', respectively.
*/
void FindingLR(double *A, double *B, double *SumofX, double *SumofY, double *SumofXY, double *SumofX2, double *SumofY2, int *Inputs)
{
    *A = (*SumofY * *SumofX2 - (*SumofX * *SumofXY)) / (*Inputs * *SumofX2 - *SumofX * *SumofX);
    *B = (*Inputs * *SumofXY - (*SumofX * *SumofY)) / (*Inputs * *SumofX2 - (*SumofX * *SumofX));
}

int main(int argc, char *argv[])
{
    // Variables used for calculating Linear Regression
    double SumofX = 0;
    double SumofY = 0;
    double SumofXY = 0;
    double SumofX2 = 0;
    double SumofY2 = 0;
    int Inputs = 0;
    int Length = 0;
    double A = 0;
    double B = 0;

    //------ Used for loops. --------
    int i = 0;
    int j = 0;
    int k = 0;

    FILE *fp;
    char *temparrayinstring = (char *)calloc(1, sizeof(char));

    /*
        This loop the program will attempt to open the file specified by 'argv[i]' in read only mode.
        If the file cannot be opened as stated in the if condition, an error message is printed and
        the program returns 1. If the file can be opened, the file pointer 'fp' is set to the open file,
        and the file's length is calculated and added to the 'Length' variable.
    */
    for (i = 1; i < argc; i++)
    {
        // If program didn't find any files with the mentioned name.
        if ((fp = fopen(argv[i], "r")) == NULL)
        {
            printf("Files not found. %s\n", argv[i]);
            return 1;
        }
        else
        {
            fp = fopen(argv[i], "r");
            fseek(fp, 0, SEEK_END);
            Length += ftell(fp);
            fseek(fp, 0, SEEK_SET);

            temparrayinstring = realloc(temparrayinstring, Length * sizeof(char));
            char chartoarray;

            /*
                This section of the program reads each character from the file, one at a time, using the 'fgetc'
                function, until the end of the file is reaches(indicated by the 'EOF'(accronym for 'End of File')).
                If the character read is a newline character \n, the 'Inputs' variable is incremented. The character
                is then added to the 'temparrayinstring' array.
            */
            for (j = 0; (chartoarray = fgetc(fp)) != EOF; j++)
            {
                if (chartoarray == '\n')
                {
                    Inputs += +1;
                }
                temparrayinstring[k] = chartoarray;
                k++;
                // printf("%c\n", chartoarray); <-- To check if program is reading all the characters.
            }
        }
    }
    // Closes fp.
    fclose(fp);

    // String Token will be used to remove unwanted charachters such as , and \n.
    char delim[] = ",\n";
    char *ptr = strtok(temparrayinstring, delim);

    // Since int takes 4 bytes a new int array is created with 4*
    int *arrayofint = calloc((Length * 4), sizeof(int));

    // Reset i to 0 for re-usability.
    i = 0;
    while (ptr != NULL)
    {
        // printf("%s\n", ptr);<-- is used to double check if the temparrayinstring[] has indeed aquired all X-axis and Y-axis form each file.
        arrayofint[i] = atoi(ptr);
        ptr = strtok(NULL, delim);
        i++;
    }

    // Main argorithm to allocate data to: Sum of X, Sum of Y, Sum of XY, Sumof X^2 and Sum of Y^2.
    for (i = 0; i < (Inputs * 2); i++)
    {
        int temp = 0;
        int XYtemp[2];
        if (i % 2 == 0)
        {
            SumofX += (double)arrayofint[i];
            temp = (double)arrayofint[i];
            SumofX2 += temp * temp;
        }
        else
        {
            SumofY += (double)arrayofint[i];
            temp = (double)arrayofint[i];
            SumofY2 += temp * temp;

            SumofXY += (arrayofint[i - 1] * arrayofint[i]);
        }
    }
    // Send the variables by reference and manipulate the data by using the method: FindingLR in line 10.
    FindingLR(&A, &B, &SumofX, &SumofY, &SumofXY, &SumofX2, &SumofY2, &Inputs);

    // Testing Data and comparing it with the excel data.

    // printf("\nA is: %f", A);
    // printf("B is: %f", B);
    // printf("Inputs :%d\n", Inputs);
    // printf("SumofX %f\n", SumofX);
    // printf("SumofY %f\n", SumofY);
    // printf("SumofXY %f\n", SumofXY);
    // printf("SumofX2 %f\n", SumofX2);
    // printf("SumofY2 %f\n", SumofY2);

    // Use y=bx+a (aka. y=mx+c) to aquire the Predicted Y.
    double WhatisY = 0;
    printf("Using the equation y=bx+a. \nFrom the data aquired we have A as %f and B as %f\nEnter X:", A, B);
    scanf("%lf", &WhatisY);
    printf("\nY = (%f * %.2f) + %f\n", B, WhatisY, A);
    printf("The predicted Y is :%f\n", (B * WhatisY) + A);

    // Freeing allocated memory.
    free(temparrayinstring);
    free(arrayofint);
    return 0;
}