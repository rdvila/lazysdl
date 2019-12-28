#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *map = fopen("lazy.map", "r");

    char *str = malloc(4 * sizeof(char));
    memset(str, 0, 16);

    int number = -1;

    int i = 0;
    while (!feof(map))
    {
        if (fgets(str, 3, map))
        {
            number = atoi(str);

            printf("%i\n", number);
            i++;
        }
    }

    free(str);
    printf("%i\n", i);

    fclose(map);

    return 0;
}

