#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

int main()
{
    char cuvant_cheie[100][100];
    int i,nr_cuvinte_cheie;
    int site_count=nr_linii_fisier("master.txt");
    site_file_info *catalog=(site_file_info*)malloc(sizeof(site_file_info)*site_count); //alocam dinamic memoria pentru numele fisierelor
    site_info *baza_de_date=(site_info*)malloc(sizeof(site_info)*site_count);  //alocam dinamic memoria pentru baza de date a site-urilor
   
    read_site_catalog(catalog); //citim fisierul cu numele site-urilor
    build_site_database(catalog, baza_de_date, site_count); //construim baza de date
   
    char linie[100],exceptie[100],secventa[100];
    int nr_site_uri;
    fgets(linie,100,stdin);
    linie[strlen(linie)-1]='\0';
    
    build_keyword_list_with_ex_seq(linie,exceptie,secventa,cuvant_cheie,&nr_cuvinte_cheie); //construim lista de cuvinte cheie si retinem secventa si exceptia in vectori 
    site_info *baza_rezultat=(site_info*)malloc(sizeof(site_info)*site_count); //alocam dinamic memorie pentru baza de date a site-urilor care contin cuvinte sau secventa cheie si nu contin exceptia
    search_keywords_with_ex_seq( baza_de_date,site_count, cuvant_cheie, nr_cuvinte_cheie,exceptie,secventa, baza_rezultat,&nr_site_uri); //punem site-urile care indeplinesc conditiile cerute in baza rezultat
    pointer_comparator func_cmp = comparator2; //initializam func_cmp
    sortare(baza_rezultat,nr_site_uri, func_cmp); //ordonam site-urile din baza rezultat
    for(i=0;i<nr_site_uri;i++)
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
