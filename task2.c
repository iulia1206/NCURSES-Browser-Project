#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

int main()
{
    FILE *f;
    char s[100],cuvant_cheie[100][100];
    int nr,i,j,nr_cuvinte_cheie;
    int site_count=nr_linii_fisier("master.txt");
    site_file_info *catalog=(site_file_info*)malloc(sizeof(site_file_info)*site_count); //alocam dinamic memoria pentru numele fisierelor
    site_info *baza_de_date=(site_info*)malloc(sizeof(site_info)*site_count);  //alocam dinamic memoria pentru baza de date a site-urilor
   
    read_site_catalog(catalog); //citim catalogul site-urilor
    build_site_database(catalog, baza_de_date, site_count); //construim baza de date
   
    char linie[100];
    fgets(linie,100,stdin);
    linie[strlen(linie)-1]='\0'; //stergem \n de la sfarsitul liniei
    nr_cuvinte_cheie = build_keyword_list(linie, cuvant_cheie); 
    site_info *baza_rezultat=(site_info*)malloc(sizeof(site_info)*site_count); //alocam dinamic memorie pentru baza de date a site-urilor care contin cuvinte cheie
    int nr1;

    search_keywords(baza_de_date, site_count, cuvant_cheie, nr_cuvinte_cheie, baza_rezultat, &nr1); //cautam cuvintele in baza de date si punem site-urile ce le contin in baza_rezultat 
    pointer_comparator func_cmp = comparator1; //initializam func_cmp
    sortare(baza_rezultat, nr1, func_cmp); //ordonam site-urile in noua baza de date 
   
    for(i=0;i<nr1;i++)
        printf("%s\n",baza_rezultat[i].URL);

    //Eliberam memoria
    for(i = 0; i < site_count; i++)
    {
        free(baza_de_date[i].continut);
    }
    free(baza_rezultat);
    free(catalog);
    free(baza_de_date);

    return 0;
}

