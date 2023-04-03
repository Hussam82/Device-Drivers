#include <stdio.h>
#include <stdlib.h>
#define PATH "/dev/test_file"

void WriteFile()
{
    char str[100] = {};
    FILE *fptr;
    fptr = fopen(PATH, "w");
    if(fptr == NULL)
    {
        printf("Could not open the file\n");
        return;
    }
    printf("Please enter data: ");
    scanf("%100[a-zA-Z ]", str);

    fprintf(fptr, "%s", str);
    fclose(fptr);
}

void ReadFile()
{
    char str[100] = {};
    FILE *fptr;
    fptr = fopen(PATH, "r");
    if (fptr == NULL)
    {
        printf("Could not open the file\n");
        return;
    }

    fscanf(fptr, "%[^\n]", str); // take everything except the \n
    printf("Message is %s\n", str);
    fclose(fptr);
}

int main()
{
    WriteFile();
    ReadFile();
    return 0;
}