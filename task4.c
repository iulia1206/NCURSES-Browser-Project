#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//structura SiteInfo contine modelul unei inregistrari din baza de date
typedef struct SiteInfo
{
    char URL[50],l_octeti[30],checksum[30],nr_accesari[30],*site_html;
} site_info; //site_info este un nou tip de date

//structura SiteFileInfo contine numele fisierului unui site
typedef struct SiteFileInfo
{
    char site_file[30];
} site_file_info; //site_file_info este un nou tip de date

//functia nr_linii_fisier calculeaza numarul de linii dintr-un fisier
int nr_linii_fisier (char *file_name)
{
    int nr =0;
    char s[100];
    FILE *f;
    f=fopen(file_name,"r");
    while (fgets(s,100,f)!=NULL)
           nr++;
    fclose(f);
    return nr;
}

//functia dim_paragraf calculeaza cate caractere se afla in codul html al site-ului
int dim_site_html (char *file_name)
{
    int dim =0;
    char s[100];
    FILE *f;
    f=fopen(file_name,"r");
    fgets(s,100,f);
    while (fgets(s,100,f)!=NULL)
        {
            dim=dim+strlen(s)+1;

        }
    fclose(f);
    return dim;
}

//functia rotr roteste la dreapta bitii lui x cu k pozitii
 int rotr(char x, int k)
{
    return ((x>>k)|(x<<(8-k)))&255;
}

//functia rotl roteste la stanga bitii lui x cu k pozitii
int rotl(char x,int k)
{
    return ((x<<k|(x>>(8-k)))&255);
}
//functia cautare verifica daca site-ul citit se regaseste in catalog
int cautare(site_info site,char new_site[30])
{
        if(strcmp(site.URL,new_site)==0)
            return 1;
    return 0;
}

int main()
{
    FILE *f;
    char s[100],*p,site[30];
    int nr,i,j;
    int site_count=nr_linii_fisier("master.txt");
    site_file_info *catalog=(site_file_info*)malloc(sizeof(site_file_info)*site_count); //alocam dinamic memoria pentru numele fisierelor
    site_info *baza_de_date=(site_info*)malloc(sizeof(site_info)*site_count);  //alocam dinamic memoria pentru baza de date a site-urilor
   
    f=fopen("master.txt","r");
    nr = 0;
    while (fgets(catalog[nr].site_file, 30, f)!=NULL)
    {
        if (catalog[nr].site_file[strlen(catalog[nr].site_file) - 1] == '\n')
        catalog[nr].site_file[strlen(catalog[nr].site_file)-1]='\0';  //eliminal \n din numele site-ului
        nr++;
    }
    fclose(f);

    for(i=0;i<site_count;i++)
    {
        int size_continut=dim_site_html(catalog[i].site_file); //determinam dimensiunea continutului codului html
        f=fopen(catalog[i].site_file,"r+");
        //completam structura
        fscanf (f, "%s", baza_de_date[i].URL);
        fscanf (f,"%s", baza_de_date[i].l_octeti);
        fscanf (f,"%s", baza_de_date[i].nr_accesari);
        fscanf (f,"%s", baza_de_date[i].checksum);
        baza_de_date[i].site_html = (char*)malloc(sizeof(char)*size_continut); //alocam dinamic memoria continutului site-ului
        //punem codul html in structura
	fgets(s,100,f);
        strcpy(baza_de_date[i].site_html,"<html>");
        baza_de_date[i].site_html[0]='\0';
        while(fgets(s,100,f)!=NULL)
        {
           strcat(baza_de_date[i].site_html,s);
        }
        if(baza_de_date[i].site_html[strlen(baza_de_date[i].site_html)-1]=='\n') //daca se gaseste, eliminam \n de la sfarsitul codului
            baza_de_date[i].site_html[strlen(baza_de_date[i].site_html)-1]='\0';
    }
    char new_site[30];
    while(fgets(new_site,30,stdin) && new_site[0]!='\n') //citim site-uri pana la intalnirea cu \n si punem site-ul curent in vectorul new_site
    {
        if(new_site[strlen(new_site)-1]=='\n') //eliminam, daca se gaseste, \n de la sfarsitul site-ului
            new_site[strlen(new_site)-1]='\0';
        int ok=0,poz;
        for(i=0;i<site_count;i++)
                if(cautare(baza_de_date[i],new_site)==1)  
                    {
                        ok=1;
                        poz=i;
                    }
        if(ok==1) //daca site-ul se gaseste in catalog
        {
            int CHEIE_GASITA=rotl(baza_de_date[poz].site_html[0],0)^rotr(baza_de_date[poz].site_html[1],1);
            int n=strlen(baza_de_date[poz].site_html);
		//calculam checksum-ul
            for(i=2;i<strlen(baza_de_date[poz].site_html);i++)
                if(i%2==0)
                    CHEIE_GASITA=CHEIE_GASITA^rotl(baza_de_date[poz].site_html[i],i%8); 
                else
                        CHEIE_GASITA=CHEIE_GASITA^rotr(baza_de_date[poz].site_html[i],i%8);
            if(CHEIE_GASITA==atoi(baza_de_date[poz].checksum)) //transformam checksum-ul de tip char in int si verificam daca sunt egale
                    printf("Website safe!\n");
            else
                printf("Malicious website! Official key: %s. Found key: %d\n",baza_de_date[poz].checksum,CHEIE_GASITA);
        }
         else
            printf("Website not found!\n");

    }
    //Eliberam memoria
    for(i = 0; i < site_count; i++)
    {
        free(baza_de_date[i].site_html);
    }
    free(catalog);
    free(baza_de_date);

    return 0;
}

