//enumerarea n_color contine culorile din libraria ncurses

enum n_color {black, red, green, yellow, blue, magenta, cyan, white, UNKNOWN};



//structura SiteInfo contine modelul unei inregistrari din baza de date

typedef struct SiteInfo

{

    char URL[50],l_octeti[10],titlu[50],checksum[30],nr_accesari[30],culoare_background[30],culoare_text[30],*continut;

} site_info; //site_info este un nou tip de date



//structura SiteFileInfo contine numele fisierului unui site

typedef struct SiteFileInfo

{

    char site_file[30];

} site_file_info; //site_file_info este un nou tip de date



//pointer_comparator este un nou tip de date

typedef int (*pointer_comparator)(site_info, site_info);



//functia nr_linii_fisier calculeaza numarul de linii dintr-un fisier

int nr_linii_fisier (char *file_name);



//functia dim_paragraf calculeaza cate caractere se afla in continutul site-ului

int dim_paragraf (char *file_name);



//functia afiseaza_baza_de_date afiseaza baza de date a lui Biju

void afiseaza_baza_de_date(site_info * baza_de_date, int site_count);



//functia read_site_catalog citeste fisierul cu numele site-urilor

void read_site_catalog(site_file_info *catalog);



//functia build_site_database construieste baza de date

void build_site_database(site_file_info *catalog, site_info *baza_de_date, int site_count);



//functia sortare face sortarea site-urilor in functie de cerinta

void sortare(site_info *baza_date, int nr_siteuri, pointer_comparator func_comparator);



//functia verificare cauta un cuvant intr-un string, daca il gaseste returneaza 1, altfel 0

int verificare(char* s1, char* s2);



//functia build_keyword_list construieste matricea cuvant_cheie unde pune cuvintele cheie

int build_keyword_list(char * linie, char cuvant_cheie[100][100]);



//functia comparator1 are ca parametri 2 structuri si returneaza un numar pozitiv daca site-urile trebuie sortate si un numar negativ daca nu trebuie sortate

int comparator1(site_info s1,site_info s2);



//functia comparator2 are ca parametri 2 structuri si returneaza un numar pozitiv daca site-urile trebuie sortate si un numar negativ daca nu trebuie sortate

int comparator2(site_info s1,site_info s2);



//functia search_keywords cauta cuvintele cheie in continutul site-urilor si daca il gaseste copiaza site-ul in structura baza_rezultat

void  search_keywords(site_info * baza_de_date, int site_count, char cuvant_cheie[100][100], int nr_cuvinte_cheie,  site_info * baza_rezultat,int *nr1);



//functia build_keyword_list_with_ex_seq construieste matricea out_cuvant_cheie unde pune cuvintele cheie, pune in vectorul out_secventa secventa si in out_exceptie exceptia

void build_keyword_list_with_ex_seq(char *in_linie, char *out_exceptie,char *out_secventa,char out_cuvant_cheie[][100],int *out_nr_cuvinte_cheie);



//functia search_keywords_with_ex_seq cauta cuvintele cheie si secventa in continutul site-urilor si verifica daca se afla exceptia; daca se indeplinesc conditiile, copiaza site-ul in structura baza_rezultat

void search_keywords_with_ex_seq(site_info * baza_de_date, int site_count, char cuvant_cheie[100][100], int nr_cuvinte_cheie,char *exceptie,char *secventa, site_info * baza_rezultat,int *nr1);

