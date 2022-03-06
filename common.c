#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

//functia translate_html_color asociaza o culoare HTML de tip string unui numar corespunzator unei culori din libraria ncurses
enum n_color translate_html_color(char * html_color)
{
    //enum n_color
    char color_list[][20] = {"black","red","green","yellow","blue","magenta","cyan","white"};
    int color_no;
    for (color_no=0; color_no < 8;color_no++)
    {
        if (strcmp(html_color, color_list[color_no]) == 0)
        {
            return color_no;
        }
    }
    return UNKNOWN;  //daca nu gasim culoarea, returnam o valoare default
}

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

//functia dim_paragraf calculeaza cate caractere se afla in continutul site-ului
int dim_paragraf (char *file_name)
{
    int dim =0;
    char s[100], *continut;
    FILE *f;
    f=fopen(file_name,"r");
    while (fgets(s,100,f)!=NULL)
    {
        if((continut = strstr(s,"<p"))!=NULL) //verificam daca linia respectiva contine inceputul paragrafului
        {
            continut += strlen("<p");
            while(*continut != '>')   //sarim peste toate caracterele din tag-ul <p...>
                continut++;
            continut++;
            char *sfarsit_continut;
            while((sfarsit_continut=strstr(s,"</p>"))==NULL &&s!=NULL)  //cautam sfarsitul paragrafului
            {
                dim += (strlen(continut)+1); //dimensiunea unei linii in octeti este numarul de caractere(strlen) + un octet pentru \0
                fgets(s,100,f);
                continut=s;
            }
            sfarsit_continut[0]='\0';  //eliminam </p> de la sfarsitul paragrafului
            dim += strlen(continut)+1;
       }
    }
    fclose(f);
    return dim;
}

//functia afiseaza_baza_de_date afiseaza baza de date a lui Biju
void afiseaza_baza_de_date(site_info * baza_de_date, int site_count)
{
    int i;
    for(i=0;i<site_count;i++)
    {
        printf("%s %s %s\n",baza_de_date[i].URL,baza_de_date[i].nr_accesari,baza_de_date[i].titlu);
    }
}

//functia read_site_catalog citeste fisierul cu numele site-urilor
void read_site_catalog(site_file_info *catalog)
{
    int nr;
    FILE * f=fopen("master.txt","r");
    nr = 0;
    while (fgets(catalog[nr].site_file, 30, f)!=NULL)
    {
       if (catalog[nr].site_file[strlen(catalog[nr].site_file) - 1] == '\n')
           catalog[nr].site_file[strlen(catalog[nr].site_file)-1]='\0';  //eliminal \n din numele site-ului
        nr++;
    }
    fclose(f);
}

//functia build_site_database construieste baza de date
void build_site_database(site_file_info *catalog, site_info *baza_de_date, int site_count)
{
    FILE *f;
    int i;
    char s[100];

    for(i=0;i<site_count;i++)
    {
        int size_continut=dim_paragraf(catalog[i].site_file); //determinam dimensiunea continutului site-ului
        f=fopen(catalog[i].site_file,"r+");
        //completez structura
        fscanf (f, "%s", baza_de_date[i].URL);
        fscanf (f,"%s", baza_de_date[i].l_octeti);
        fscanf (f,"%s", baza_de_date[i].nr_accesari);
        fscanf (f,"%s", baza_de_date[i].checksum);
        baza_de_date[i].titlu[0]='\0'; //ne asiguram ca titlul este gol
        while(fgets(s,100,f)!=NULL)
        {
            char *titlu,*continut,*culoare_background,*culoare_text;
            if((titlu = strstr(s,"<title"))!=NULL)  //cautam tag-ul de inceput al titlului in linia curenta
            {
                titlu += strlen("<title");  //sarim peste toate caracterele din tag-ul <title...>
                char *sfarsit_titlu;
                while(*titlu != '>')
                    titlu++;
                titlu++;
                while((sfarsit_titlu=strstr(s,"</title>"))==NULL &&s!=NULL)  //cautam sfarsitul titlului
                {
                    strcat(baza_de_date[i].titlu,titlu); //adaugam o linie(in cazul in care sunt mai multe) care nu contine sfarsitul titlului la valoarea titlului din structura
                    fgets(s,100,f);
                    titlu=s;
                }
                sfarsit_titlu[0]='\0';  //eliminam </title> de la sfarsitul paragrafului
                strcat(baza_de_date[i].titlu,titlu);  //adaugam ultima linie din titlu(in cazul in care sunt mai multe) in structura
            }
            if((continut = strstr(s,"<p"))!=NULL)  //verificam daca linia respectiva contine inceputul paragrafului
            {
                baza_de_date[i].continut = (char*)malloc(sizeof(char)*size_continut); //alocam dinamic memoria continutului site-ului
                baza_de_date[i].continut[0]='\0';
                continut += strlen("<p");
                while(*continut != '>')  //sarim peste toate caracterele din tag-ul <p...>
                    continut++;
                continut++;
                char *sfarsit_continut;
                while((sfarsit_continut=strstr(s,"</p>"))==NULL &&s!=NULL)  //cautam sfarsitul paragrafului
                {
                    strcat(baza_de_date[i].continut,continut);  //adaugam o linie la valoarea continutului din structura
                    fgets(s,100,f);
                    continut=s;
                }
                sfarsit_continut[0]='\0';  //eliminam </p> de la sfarsitul paragrafului
                strcat(baza_de_date[i].continut,continut);  //adaugam ultima linie de text in valoarea continutului din structura
            }
        }
        fclose(f);
    }
    for(i=0;i<site_count;i++)   
    {
        f=fopen(catalog[i].site_file,"r+");  //mai deschidem inca o data fisierele
        while(fgets(s,100,f)!=NULL)
        {
		//punem in vectorul text_stile tot ce se afla intre "<p" si ">"
            char *culoare_background,*culoare_text,text_style[100],*style;
            text_style[0]='\0';
            if((style=strstr(s,"<p"))!=NULL)
                {
                    style += strlen("<p");
                    char *sfarsit_style;
                    while((sfarsit_style=strstr(s,">"))==NULL) //caut sfarsitul tag-ului p 
                    {
                        strcat(text_style,style);  
                        fgets(s,100,f);
                        style=s;
                    }
                    sfarsit_style[0]='\0';
                    strcat(text_style,style);
                }
            if(strstr(s,"<p")!=NULL) //cautam inceputul tagului p
             {
                if((culoare_background=strstr(text_style,"background-color:"))!=NULL) //daca are culoare de background
                {
                    baza_de_date[i].culoare_background[0]='\0';
                    culoare_background += strlen("background-color:");  //trecem peste "background-color:"
                    char *p;
                    p=strtok(culoare_background,";");
                    strcat(baza_de_date[i].culoare_background,p);
                }
               else
               {
                    strcpy(baza_de_date[i].culoare_background,"UNKNOWN"); //daca nu gaseste completam cu UNKNOWN 
                    if((culoare_text=strstr(text_style,"color:"))!=NULL) //daca gaseste culoarea textului
                    {
                        baza_de_date[i].culoare_text[0]='\0';
                        culoare_text += strlen("color:");  //trecem peste "color:"
                        char *p;
                        p=strtok(culoare_text,";");
                        strcat(baza_de_date[i].culoare_text,p);
                    }
                    else
                        strcpy(baza_de_date[i].culoare_text,"UNKNOWN"); //daca nu gaseste completam cu UNKNOWN
               }
               if(strcmp(baza_de_date[i].culoare_background,"UNKNOWN")!=0) //daca este gasita culoarea de background cautam si culoarea pentru text
               {
                    if((culoare_text=strstr(text_style,"\"color:"))!=NULL)
                    {
                        baza_de_date[i].culoare_text[0]='\0';
                        culoare_text += strlen("\"color:"); ////trecem peste "\"color:"
                        char *p;
                        p=strtok(culoare_text,";");
                        strcat(baza_de_date[i].culoare_text,p);
                    }
                    else
                        strcpy(baza_de_date[i].culoare_text,"UNKNOWN"); ////daca nu gaseste completam cu UNKNOWN
               }
             }
        }
        fclose(f);
    }
}

//functia sortare face sortarea site-urilor in functie de cerinta
void sortare(site_info *baza_date, int nr_siteuri, pointer_comparator func_comparator)
{
    int i, j;
    for(i=0; i< nr_siteuri-1; i++)
    {
        for(j = i+1; j < nr_siteuri; j++)
        {
                if (func_comparator(baza_date[i], baza_date[j]) > 0)  //interschimbam site-urile din baza de date daca func_comparator returneaza un numar pozitiv
                {
                    site_info  aux;
                    memcpy(&aux, &baza_date[i], sizeof(site_info));
                    memcpy(&baza_date[i], &baza_date[j], sizeof(site_info));
                    memcpy(&baza_date[j],&aux, sizeof(site_info));
                }
        }
    }
}

//functia verificare cauta un cuvant intr-un string, daca il gaseste returneaza 1, altfel 0
int verificare(char* s1, char* s2)
 {
	int i, j, n, m;
	n = strlen(s1);
	m = strlen(s2);
	if (m > n)  //daca acel cuvant are mai multe caractere decat stringul atunci clar nu se regaseste
		return 0;
    i=0;
	while (i < n)
    {
		j = 0;
		while (i < n && j < m && s1[i] == s2[j]) //parcurgem caracterele identice din cele 2 siruri
		{
			i++;
			j++;
		}
		//verificam daca am ajuns la sfarsitul cuvantului din primul sir si daca am parcurs toate caracterele cuvantului pe care il cautam
		if (( i == n || s1[i] ==' ' || s1[i]=='.' || s1[i]==',' || s1[i]==':' || s1[i]==';' || s1[i]=='"' || s1[i]=='\'' || s1[i]=='\n') && j == m)
			return 1;
	        //trecem la urmatorul cuvant din sirul 1
		while (i < n && s1[i] !=' ' && s1[i]!='.' && s1[i]!=',' && s1[i]!=':' && s1[i]!=';' && s1[i]!='"' && s1[i]!='\'' && s1[i]!='\n')
			i++;
		while (i < n && (s1[i] ==' ' || s1[i]=='.' || s1[i]==',' || s1[i]==':' || s1[i]==';' || s1[i]=='"' || s1[i]=='\'' || s1[i]=='\n'))
			i++;
	}
	return 0;
}

//functia comparator1 are ca parametri 2 structuri si returneaza un numar pozitiv daca site-urile trebuie sortate si un numar negativ daca nu trebuie sortate
int comparator1(site_info s1,site_info s2)
{
    int r = strcmp(s1.continut, s2.continut);
    if ( r == 0) //daca continuturile sunt identice
        if (strlen(s1.nr_accesari) > strlen(s2.nr_accesari)) //daca numarul de caractere dintr-un string cu numarul de accesari e mai mare atunci numarul de accesari este mai mare
             return -1;
        else
            if(strlen(s1.nr_accesari) == strlen(s2.nr_accesari))  //daca string-urile au acelasi numar de caractere se va aplica aceeasi metoda ca la titlu
                return strcmp(s2.nr_accesari,s1.nr_accesari);
            else
                return 1;
    return r;
}

//functia comparator2 are ca parametri 2 structuri si returneaza un numar pozitiv daca site-urile trebuie sortate si un numar negativ daca nu trebuie sortate
int comparator2(site_info s1,site_info s2)
{

    if (strlen(s1.nr_accesari) > strlen(s2.nr_accesari)) //daca numarul de caractere dintr-un string cu numarul de accesari e mai mare atunci numarul de accesari este mai mare
         return -1;
    else
        if(strlen(s1.nr_accesari) == strlen(s2.nr_accesari))  //daca string-urile au acelasi numar de caractere atunci verificarea se va face strcmp
            return strcmp(s2.nr_accesari,s1.nr_accesari);
        else
            return 1;

}

//functia build_keyword_list construieste matricea cuvant_cheie unde pune cuvintele cheie si returneaza numarul lor
int build_keyword_list(char * linie, char cuvant_cheie[100][100])
{
    int nr_cuvinte_cheie;
    char *p;
    p=strtok(linie," ");
    nr_cuvinte_cheie=0;
    while(p!=NULL)                //punem cuvintele cheie in matricea cuvant_cheie
    {
        strcpy(cuvant_cheie[nr_cuvinte_cheie],p);
        p=strtok(NULL," ");
        nr_cuvinte_cheie++;
    }

    return nr_cuvinte_cheie;
}

//functia search_keywords cauta cuvintele cheie in continutul site-urilor si daca il gaseste copiaza site-ul in structura baza_rezultat
void search_keywords(site_info * baza_de_date, int site_count, char cuvant_cheie[100][100], int nr_cuvinte_cheie,  site_info * baza_rezultat,int *nr1)
{
    int i,j,*pozitie;

    *nr1=0;
    pozitie=(int*)malloc(sizeof(int)*site_count); //alocam dinamic memorie pentru vectorul de frecventa
    for(i=0;i<site_count;i++) //initializam vectorul de frecventa cu 0
        pozitie[i]=0;
    for(i=0;i<nr_cuvinte_cheie;i++)
        for(j=0;j<site_count;j++)
        {
            if(verificare(baza_de_date[j].continut,cuvant_cheie[i])==1) //daca cuvantul cheie se regaseste in continutul site-ului atunci frecventa pozitiei lui creste
            {
                pozitie[j]++;
            }
        }
    for(i=0;i<site_count;i++)
        if(pozitie[i]!=0)   //daca continutul site-ului contine cuvantul cheie atunci copiem detaliile site-ului intr-o baza de date intermediara
    {
        memcpy(&baza_rezultat[*nr1],&baza_de_date[i],sizeof(site_info));
        (*nr1)++;
    }

    free(pozitie);
}

//functia build_keyword_list_with_ex_seq construieste matricea out_cuvant_cheie unde pune cuvintele cheie, pune in vectorul out_secventa secventa si in out_exceptie exceptia
void build_keyword_list_with_ex_seq(char *in_linie, char *out_exceptie,char *out_secventa,char out_cuvant_cheie[][100],int *out_nr_cuvinte_cheie)
{
    char copie_linie[100],*p;
    int i;
    out_exceptie[0]='\0';
    out_secventa[0]='\0';
    strcpy(copie_linie,in_linie);
    if(strchr(copie_linie,'"')!=NULL)  //daca exista, punem secventa data intre "" in vectorul secventa
    {
        p=strtok(copie_linie,"\"");
        p=strtok(NULL,"\"");
        strcpy(out_secventa,p);
    }
    strcpy(copie_linie,in_linie);
    if(strchr(copie_linie,'-')!=NULL)  //daca exista, punem exceptia data in vectorul exceptie
    {
        p=strtok(copie_linie,"-");
        while(p!=NULL)
       {
            strcpy(out_exceptie,p);
            p=strtok(NULL,"-");
       }
    }
    for(i=0;i<strlen(in_linie);i++)  //daca gasim "" sau "-" eliminam tot ce urmeaza pentru a ramane doar cu cuvintele cheie
        if(in_linie[i]=='"')
        {
            in_linie[i]='\0';
            break;
        }
        else
            if(in_linie[i]=='-' && in_linie[i-1]==' ')
            {
                in_linie[i]='\0';
                break;
            }
    *out_nr_cuvinte_cheie=0;
    p=strtok(in_linie," ");
    while(p!=NULL)     //punem cuvintele cheie in matricea cuvant_cheie
   {
        strcpy(out_cuvant_cheie[*out_nr_cuvinte_cheie],p);
        (*out_nr_cuvinte_cheie)++;
        p=strtok(NULL," ");
   }
}
//functia search_keywords_with_ex_seq cauta cuvintele cheie si secventa in continutul site-urilor si verifica daca se afla exceptia; daca se indeplinesc conditiile, copiaza site-ul in structura baza_rezultat
void search_keywords_with_ex_seq(site_info * baza_de_date, int site_count, char cuvant_cheie[100][100], int nr_cuvinte_cheie,char *exceptie,char *secventa, site_info * baza_rezultat,int *nr1)
{
    int nr_site_uri=0,*pozitie,i,j;
    pozitie=(int*)malloc(sizeof(int)*site_count); //alocam dinamic memorie pentru vectorul de frecventa
    for(i=0;i<site_count;i++) //initializam vectorul de frecventa cu 0
        pozitie[i]=0;
    for(i=0;i<nr_cuvinte_cheie;i++)
        for(j=0;j<site_count;j++)
        {
            if(verificare(baza_de_date[j].continut,cuvant_cheie[i])==1) //daca cuvantul cheie se regaseste in continutul site-ului atunci frecventa pozitiei lui creste
            {
                pozitie[j]++;
            }
        }
    if(secventa[0]!=NULL)
        for(j=0;j<site_count;j++)
        {
            if(strstr(baza_de_date[j].continut,secventa)!=NULL) //daca secventa se regaseste in continutul site-ului atunci frecventa pozitiei lui creste
            {
                pozitie[j]++;
            }
        }
    if(exceptie[0]!=NULL)
        for(j=0;j<site_count;j++)
        {
            if(verificare(baza_de_date[j].continut,exceptie)==1) //daca exceptia se regaseste in continutul site-ului atunci frecventa pozitiei lui devine 0
            {
                pozitie[j]=0;
            }
        }
    for(i=0;i<site_count;i++)
        if(pozitie[i]!=0)   //daca continutul site-ului contine un cuvant cheie sau secventa cheie dar nu si exceptia atunci copiem detaliile site-ului intr-o baza de date intermediara
    {
        memcpy(&baza_rezultat[nr_site_uri],&baza_de_date[i],sizeof(site_info));
        nr_site_uri++;
    }
    *nr1=nr_site_uri;
    free(pozitie);
}
