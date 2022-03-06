#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
int main()
{
    int i;
    int site_count=nr_linii_fisier("master.txt");
    site_file_info *catalog=(site_file_info*)malloc(sizeof(site_file_info)*site_count); //alocam dinamic memoria pentru numele fisierelor
    site_info *baza_de_date=(site_info*)malloc(sizeof(site_info)*site_count);  //alocam dinamic memoria pentru baza de date a site-urilor

    // Citim catalogul de site-uri
    read_site_catalog(catalog);

    // Construim baza de date
    build_site_database(catalog, baza_de_date, site_count);

    // Afisem baza de date
    afiseaza_baza_de_date(baza_de_date, site_count);

    //Eliberam memoria
    for(i = 0; i < site_count; i++)
    {
        free(baza_de_date[i].continut);
    }
    free(catalog);
    free(baza_de_date);
    return 0;
}

