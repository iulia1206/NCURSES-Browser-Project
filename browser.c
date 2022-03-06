    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ncurses.h>
    #include <menu.h>
    #include "common.h"

    //functia show_web_page afiseaza un site web curespunzator parametrilor transmisi prin structura site_info
    void  show_web_page(site_info * page_info)
    {
    //creem fereastra web_window
    WINDOW *web_window;
    int  line_no =  4, column_no = 2, i,maxl,maxc;
    web_window= newwin(0, 0, 0, 0); //o initializam pe tot ecranul
    getmaxyx(web_window,maxl,maxc);
    mvwprintw(web_window,maxl-3,4,"  Lengenda: \n   'B' - Back"); //afisam legenda paginii
    box(web_window, 0 , 0); // ii facem chenar paginii
    //boldam titlul
    wattron(web_window,A_BOLD);
    mvwprintw(web_window, 2, 2, "%s", page_info->titlu);
    wattroff(web_window,A_BOLD);
    //optinem culorile din libraria ncurses corespunzatoare culorilor asignate paragrafului
    enum n_color text_color = translate_html_color(page_info->culoare_text);
    if (text_color == UNKNOWN)
        text_color = black;
    enum n_color back_color = translate_html_color(page_info->culoare_background);
    if (back_color == UNKNOWN)
        back_color = white;
        //intram in modul de afisare cu culori
    start_color();
    init_pair(1, text_color, back_color);
    wattron(web_window,COLOR_PAIR(1));
    for(i=0; i < strlen(page_info->continut);  i++)
    {
        if (page_info->continut[i] == '\n')
        {
            line_no++;
            column_no = 2;
        }
        else
        {
            mvwprintw(web_window, line_no, column_no, "%c", page_info->continut[i]);
            column_no++;
        }
    }
    wattroff(web_window,COLOR_PAIR(1)); //iesim din modul afisare cu culori
    wrefresh(web_window);

    while(wgetch(web_window) != 'B') ; //asteptam introducerea tastei 'B' pentru a reveni in fereastra precedenta
    //stergem continutul ferestrei curente si dealocam memoria
    wclear(web_window);
    wrefresh(web_window);
    delwin(web_window);
    }

    //functia show_search_results creeaza si afiseaza fereastra cu rezultatele cautarii
    void show_search_results(char metoda_cautare, char * linie, site_info * baza_de_date, int site_count)
    {
    WINDOW *result_window;
    ITEM **menu_items;
    int c, nr1;
    MENU *menu;
    char cuvainte_cheie[100][100], exceptie[100], secventa[100];
    int i, nr_cuvinte_cheie,maxl,maxc;
    pointer_comparator func_cmp;
    //creem fereastra result_window
    result_window= newwin(0, 0, 0, 0);
    getmaxyx(result_window,maxl,maxc);
    //afisam legenda ferestrei 
    mvwprintw(result_window,maxl-6,4,"  Lengenda: \n   'ENTER' - Vizualizare pagina web\n   'UP' - Navigare in sus\n   'DOWN' - Navigare in jos\n   'B' - Back");
    box(result_window, 0 , 0);
    keypad(result_window, TRUE); //permite procesarea tastelor UP,DOWN
    site_info *baza_rezultat=(site_info*)malloc(sizeof(site_info)*site_count); //alocam dinamic memoria pentru baza rezultat
    mvwprintw(result_window,2,1,"Rezultatele cautarii: %s", linie);
    //realizam cautarea in functie de metoda de cautare
    if (metoda_cautare == 'S')
    {
        nr_cuvinte_cheie = build_keyword_list(linie, cuvainte_cheie);
        wmove(result_window, 1, 1);
        search_keywords(baza_de_date, site_count, cuvainte_cheie, nr_cuvinte_cheie, baza_rezultat, &nr1);
        func_cmp = comparator1; //initializam func_cmp
    }
    else
        if (metoda_cautare == 'A')
        {
            build_keyword_list_with_ex_seq(linie, exceptie, secventa, cuvainte_cheie, &nr_cuvinte_cheie);
            wmove(result_window, 1, 1);
            search_keywords_with_ex_seq(baza_de_date, site_count, cuvainte_cheie, nr_cuvinte_cheie, exceptie, secventa, baza_rezultat, &nr1);
            func_cmp = comparator2; //initializam func_cmp
        }
    sortare(baza_rezultat, nr1, func_cmp); //ordonam rezultatele in functie de cerinta
    //alocam dinamic memoria pentru optiunile de meniu corespunzatoare rezultatelor cautarii 
    menu_items = (ITEM **)calloc(nr1 + 1, sizeof(ITEM *)); 
    for(i=0;i<nr1;i++)
    {
        menu_items[i] = new_item(baza_rezultat[i].URL,baza_rezultat[i].titlu);
    }
    menu_items[nr1] = (ITEM *)NULL;
    //creem meniul si il asociem cu fereastra result_window
    menu=new_menu((ITEM **)menu_items);
    set_menu_win(menu, result_window);
    //incadram meniul intr-o subfereastra a lui result_window pentru a fi afisat la pozitia dorita
    set_menu_sub(menu, derwin(result_window, nr1, 50, 4, 2)); 
    //afisam meniul
    post_menu(menu); 
    wrefresh(result_window);
    int menu_index = 0; //retine indexul optiunilor de meniu selectate
    //procesam tastele introduse pana la apasarea lui 'B'
    while((c = wgetch(result_window)) != 'B')
    {       switch(c)
            {	case KEY_DOWN:                        //schimbam optiunea de meniu selectata "in jos"
                menu_driver(menu, REQ_DOWN_ITEM);
                if (menu_index < nr1-1)
                    menu_index++;
                break;
                case KEY_UP:                          //schimbam optiunea de meniu selectata "in sus"
                menu_driver(menu, REQ_UP_ITEM);   
                if (menu_index > 0)
                    menu_index--;
                break;
                case '\n':                                       //afisam fereastra corespunzatoare site-ului selectat prin tasta 'ENTER'
                    show_web_page(&baza_rezultat[menu_index]);  
                    redrawwin(result_window);     //reafisam fereastra cu rezultatele cautarii
                break;
            }
            wrefresh(result_window);
    }
    //eliberam memoria alocata 
    unpost_menu(menu);
    free_menu(menu);  
    for(i = 0; i < nr1; ++i)
        free_item(menu_items[i]);

    wclear(result_window);
    wrefresh(result_window);
    free(baza_rezultat);
    delwin(result_window);
    }
    //functia show_search_screen afiseaza fereastra de cautare 
    void show_search_screen(site_info * baza_de_date, int site_count)
    {
    int i, coordonata_linie,coordonata_coloana,maxl,maxc;
    //initializam fereastra
    WINDOW *search_window = newwin(0, 0, 0, 0);
    getmaxyx(search_window,maxl,maxc);
    mvwprintw(search_window,maxl-6,4,"  Lengenda: \n   'C' - Cautare\n   'S' - Cautare simpla\n   'A' - Cautare avansata\n   'Q' - Quit"); //afisam legenda ferestrei
    mvwprintw(search_window,1,(maxl+maxc)/2-strlen("BROWSERUL LUI BIJU"),"BROWSERUL LUI BIJU");
    mvwprintw(search_window,3,1, "Introduceti actiunea dorita(C/Q):");
    box(search_window, 0 , 0);
    wrefresh(search_window);
    getyx(search_window,coordonata_linie,coordonata_coloana);
    char comanda;
    comanda=wgetch(search_window);
    waddch(search_window,comanda);
    wmove(search_window,coordonata_linie,coordonata_coloana);
    wrefresh(search_window);
    //citim in continuare caractere pana intalnim 'C' sau 'Q'
    while(comanda!='C' && comanda!='Q')
    {
        wmove(search_window,coordonata_linie,coordonata_coloana);
        comanda=wgetch(search_window);
        waddch(search_window,comanda);
        wrefresh(search_window);
    }
    char cuvinte_cheie[100],c;
    const int line_size = 100;
    //daca citim caracterul 'C' initiem citirea cuvintelor cheie
    if(comanda=='C')
    {
        mvwprintw(search_window,4,1,"Introduceti cuvintele cheie:");
        wmove(search_window,5,1);
        for(i=0;i<line_size;i++)
           waddch(search_window,' ' | A_UNDERLINE);   //creem bara de cautare

        i=0;
        wmove(search_window,5,1);
        wrefresh(search_window);
        while((c=wgetch(search_window))!='\n' && i< line_size-1)
        {
                cuvinte_cheie[i]=c;
                waddch(search_window,c | A_UNDERLINE);  //pastram modul de afisare a barii de cautare 
                wrefresh(search_window);
                i++;
        }
        cuvinte_cheie[i]='\0'; //completam cu terminatorul de sir
        mvwprintw(search_window,7,1,"Selectati metoda de cautare(S/A):");
        wrefresh(search_window);
        getyx(search_window,coordonata_linie,coordonata_coloana);
        char metoda_cautare;
        //asteptam introducerea metodei de cautare sau a tastei 'Q' pentru a iesi
        while((metoda_cautare=wgetch(search_window) )!= 'Q')
        {
            waddch(search_window,metoda_cautare);
            wmove(search_window,coordonata_linie,coordonata_coloana);
            wrefresh(search_window);
            //alegem metoda de cautare,simpla sau avansata
            if (metoda_cautare == 'S' || metoda_cautare == 'A')
            {
                show_search_results(metoda_cautare,cuvinte_cheie,baza_de_date,site_count);
                redrawwin(search_window);
                wrefresh(search_window);
            }
        }
        wrefresh(search_window);
        delwin(search_window);
    }
}


    int main()
    {
    int site_count=nr_linii_fisier("master.txt");
    site_file_info *catalog=(site_file_info*)malloc(sizeof(site_file_info)*site_count); //alocam dinamic memoria pentru numele fisierelor
    site_info *baza_de_date=(site_info*)malloc(sizeof(site_info)*site_count);  //alocam dinamic memoria pentru baza de date a site-urilor

    read_site_catalog(catalog);  //citim catalogul site-urilor
    build_site_database(catalog, baza_de_date, site_count);  //contruim baza de date
    int i;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr,TRUE);
    refresh();
    show_search_screen(baza_de_date, site_count); 
    refresh();
    endwin();
    //Eliberam memoria
    for(i = 0; i < site_count; i++)
    {
        free(baza_de_date[i].continut);
    }
    free(catalog);
    free(baza_de_date);
    return 0;
}

