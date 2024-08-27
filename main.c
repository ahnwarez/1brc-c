#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "hashmap.h"

const int MAX_CITY_NAME = 100;
struct station
{
    char name[MAX_CITY_NAME];
    float min;
    float max;
    long count;
    double sum;
};

typedef struct station station_t;

int station_compare(const void *a, const void *b, void *udata)
{
    const station_t *ua = a;
    const station_t *ub = b;
    return strcmp(ua->name, ub->name);
}

bool station_iter(const void *item)
{
    const station_t *st = item;
    printf("%s\n", st->name);
    printf("    %f %f %f %ld\n", st->min, st->max, st->sum, st->count);
    return true;
}

uint64_t station_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
    const station_t *user = item;
    return hashmap_sip(user->name, strlen(user->name), seed0, seed1);
}

// Don't forget to free the memory when you're done
void free_stations(struct hashmap *map)
{
    size_t iter = 0;
    void *item;
    while (hashmap_iter(map, &iter, &item))
    {
        free(item);
    }
}

void set_station(struct hashmap *map, const char *name, float temperature)
{
    station_t temp = {0};
    strncpy(temp.name, name, MAX_CITY_NAME - 1);
    temp.name[MAX_CITY_NAME - 1] = '\0'; // Ensure null-termination

    const station_t *temp_existing = hashmap_get(map, &temp);
    station_t *existing = (station_t *)temp_existing;
    if (existing != NULL)
    {
        if (existing->min > temperature)
        {
            existing->min = temperature;
        }
        if (existing->max < temperature)
        {
            existing->max = temperature;
        }
        existing->count++;
        existing->sum += temperature;
    }
    else
    {
        station_t *new_st = malloc(sizeof(station_t));
        strncpy(new_st->name, name, MAX_CITY_NAME - 1);
        new_st->name[MAX_CITY_NAME - 1] = '\0'; // Ensure null-termination
        new_st->min = temperature;
        new_st->max = temperature;
        new_st->count = 1;
        new_st->sum = temperature;

        hashmap_set(map, new_st);
    }
}

int main(int argc, char *argv[])
{
    const char *filename = "measurement.txt";
    if (argc > 1)
    {
        filename = argv[1];
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        return 1;
    }
    struct hashmap *map = hashmap_new(sizeof(struct station), 0, 0, 0,
                                      station_hash, station_compare, NULL, NULL);
    char line[MAX_CITY_NAME + 20]; // Enough space for city name, semicolon, and temperature
    while (fgets(line, sizeof(line), file))
    {
        char city[MAX_CITY_NAME];
        float temperature;

        if (sscanf(line, "%[^;];%f", city, &temperature) == 2)
        {
            set_station(map, city, temperature);
        }
        else
        {
            printf("Error parsing line: %s", line);
        }
    }

    fclose(file);

    size_t iter = 0;
    void *item;
    while (hashmap_iter(map, &iter, &item))
    {
        const station_t *user = item;
        station_iter(item);
        // printf("%s\n", user->name);
    }

    hashmap_free(map);
    return 0;
}
