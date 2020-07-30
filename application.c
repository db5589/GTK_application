#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include <sqlite3.h>
#include <time.h>

//windows 
GtkWidget *windowStack;
//fixed
GtkWidget *fixed1; GtkWidget *fixed2; GtkWidget *fixed3; GtkWidget *fixed4; GtkWidget *fixed5;
//buttons
GtkWidget *button0; GtkWidget *button1; GtkWidget *button2; GtkWidget *button3; GtkWidget *button4; GtkWidget *button5; GtkWidget *button6; GtkWidget *button7; GtkWidget *button8; GtkWidget *button9; GtkWidget *button10; GtkWidget *buttondel;
GtkWidget *buttonpower; GtkWidget *buttonminus; GtkWidget *buttonsame; GtkWidget *buttonplus;
//Labels
GtkWidget *label1; 
GtkBuilder *builder;
//calendar
GtkCalendar *calendar1;
//iconView
GtkIconView *iconView1;
//SQL
GtkTreeViewColumn *column1; GtkTreeViewColumn *column2; GtkListStore *store; GtkTreeSelection *treeSelection; GtkCellRenderer *renderer;
GtkTreeView *ColumnList; GtkEntry *entry1; GtkWidget *buttonAddEvent; GtkWidget *buttonClearEvent;

// calculator  
char numstr[100]; int i = 0; long num1 = 0; long num2 = 0;
int same = 0; int operatorSet = 0; int operation = 0; long result = 0;

// SQL variables 
int callback(void *, int, char **, char **);
enum {LIST_ID, LIST_TODO, N_COLUMNS};
sqlite3 *db; //call for db file
char *err_msg = 0;
// ----------------------------- main function ------------------------------------------------ 
int main(int argc, char *argv[]) {
    
    gtk_init(&argc, &argv); //init Gtk

    builder = gtk_builder_new_from_file ("application.glade"); //pointer to a widget
    windowStack = GTK_WIDGET(gtk_builder_get_object(builder, "windowStack")); //building the witgets for Calculator

    g_signal_connect(windowStack, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL); gtk_builder_connect_signals(builder, NULL);

    // ----------------------------- calculator ----------------------------- 
    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1")); fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2")); fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3")); fixed4 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed4")); fixed5 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed5")); 
    button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1")); button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2")); button3 = GTK_WIDGET(gtk_builder_get_object(builder, "button3")); button4 = GTK_WIDGET(gtk_builder_get_object(builder, "button4")); button5 = GTK_WIDGET(gtk_builder_get_object(builder, "button5")); button6 = GTK_WIDGET(gtk_builder_get_object(builder, "button6")); button7 = GTK_WIDGET(gtk_builder_get_object(builder, "button7")); button8 = GTK_WIDGET(gtk_builder_get_object(builder, "button8")); button9 = GTK_WIDGET(gtk_builder_get_object(builder, "button9")); button0 = GTK_WIDGET(gtk_builder_get_object(builder, "button0"));
    buttondel = GTK_WIDGET(gtk_builder_get_object(builder, "buttondel")); buttonpower = GTK_WIDGET(gtk_builder_get_object(builder, "buttonpower")); buttonminus = GTK_WIDGET(gtk_builder_get_object(builder, "buttonminus")); buttonplus = GTK_WIDGET(gtk_builder_get_object(builder, "buttonplus")); buttonsame = GTK_WIDGET(gtk_builder_get_object(builder, "buttonsame"));
    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1")); ColumnList = GTK_TREE_VIEW(gtk_builder_get_object(builder, "ColumnList")); entry1 = GTK_ENTRY(gtk_builder_get_object(builder, "entry1")); buttonAddEvent = GTK_WIDGET(gtk_builder_get_object(builder, "buttonAddEvent")); buttonClearEvent = GTK_WIDGET(gtk_builder_get_object(builder, "buttonClearEvent"));
    calendar1 = GTK_CALENDAR(gtk_builder_get_object(builder, "calendar1")); treeSelection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "treeSelection"));
    
    // ----------------------------- Iconview ----------------------------- 
    iconView1 = GTK_ICON_VIEW(gtk_builder_get_object(builder, ""));

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    gtk_calendar_select_month(GTK_CALENDAR (calendar1), tm.tm_mon, tm.tm_year + 1900 );
    gtk_calendar_select_day(GTK_CALENDAR (calendar1), tm.tm_mday);

    // SQL ----------------------------------------------------------------------------
    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) { fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db)); sqlite3_close(db); return 1;} else {printf("Database sucessuflly open\n");}

    char *sql = "DROP TABLE IF EXISTS Friends;" 
    "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);"
    "INSERT INTO Friends(Name) VALUES ('Tom');"
    "INSERT INTO Friends(Name) VALUES ('Rebecca');"
    "INSERT INTO Friends(Name) VALUES ('Jim');"
    "INSERT INTO Friends(Name) VALUES ('Roger');"
    "INSERT INTO Friends(Name) VALUES ('Robert');";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    sql = "SELECT * FROM Friends";
    rc = sqlite3_exec(db, sql, callback, store, &err_msg);

    if (rc != SQLITE_OK ) { fprintf(stderr, "Failed to select data\n"); fprintf(stderr, "SQL error: %s\n", err_msg); sqlite3_free(err_msg); sqlite3_close(db); return 1;}
    sqlite3_close(db);

    // CREATE 2 COLUMNS WITH TEXT CELL RENDERERS
    renderer = gtk_cell_renderer_text_new(); column1 = gtk_tree_view_column_new_with_attributes("Id", renderer, "text", LIST_ID, NULL); 
    gtk_tree_view_append_column(GTK_TREE_VIEW(ColumnList), column1); 

    renderer = gtk_cell_renderer_text_new(); column2 = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", LIST_TODO, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ColumnList), column2);

    gtk_tree_view_set_model(GTK_TREE_VIEW(ColumnList), GTK_TREE_MODEL(store));// Sets the model for a GtkTreeView. If the tree_view already has a model set, it will remove it before setting the new model. If model is NULL, then it will unset the old model 
    g_object_unref(store); // TREEVIEW WILL KEEP A REFERENCE SO DECREASE REFCOUNT

    // SETUP THE UI
    gtk_window_set_title(GTK_WINDOW(windowStack), "Test application");
    gtk_window_set_position(GTK_WINDOW(windowStack), GTK_WIN_POS_CENTER);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ColumnList), FALSE);
    // ----------------------------- Icon view -----------------------------


    // ----------------------------- main event -----------------------------
    gtk_widget_show(windowStack); //window must be visible 
    gtk_main(); //watch for ewent
        return EXIT_SUCCESS;
}

// ----------------------------- SQL function ------------------------------------------------------
int callback(void *model, int argc, char **argv, char **azColName) {
   GtkTreeIter iter;

   for (int i = 0; i < argc; i++) {
      printf("azColName %s =\n argv %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   // AFTER PRINTING TO CONSOLE FILL THE MODEL WITH THE DATA
   gtk_list_store_append (GTK_LIST_STORE(model), &iter);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter, LIST_ID, argv[0], LIST_TODO, argv[1], -1);

   return 0;
}
// ----------------------------- Calculator function ----------------------------- 

void on_button0_clicked (GtkButton *b) {
    if (i!=0){
    numstr[i]='0';
    if (operatorSet == 0){num1 = 10 * num1 + 0;} else {num2 = 10 * num2 + 0;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;}
}

void on_button1_clicked (GtkButton *b) {
    numstr[i]='1';
    if (operatorSet == 0){num1 = 10 * num1 + 1;} else {num2 = 10 * num2 + 1;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button2_clicked (GtkButton *b) {
    numstr[i]='2';
    if (operatorSet == 0){num1 = 10 * num1 + 2;} else {num2 = 10 * num2 + 2;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button3_clicked (GtkButton *b) {
    numstr[i]='3';
    if (operatorSet == 0){num1 = 10 * num1 + 3;} else {num2 = 10 * num2 + 3;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button4_clicked (GtkButton *b) {
    numstr[i]='4';
    if (operatorSet == 0){num1 = 10 * num1 + 4;} else {num2 = 10 * num2 + 4;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button5_clicked (GtkButton *b) {
    numstr[i]='5';
    if (operatorSet == 0){num1 = 10 * num1 + 5;} else {num2 = 10 * num2 + 5;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button6_clicked (GtkButton *b) {
    numstr[i]='6';
    if (operatorSet == 0){num1 = 10 * num1 + 6;} else {num2 = 10 * num2 + 6;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button7_clicked (GtkButton *b) {
    numstr[i]='7';
    if (operatorSet == 0){num1 = 10 * num1 + 7;} else {num2 = 10 * num2 + 7;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button8_clicked (GtkButton *b) {
    numstr[i]='8';
    if (operatorSet == 0){num1 = 10 * num1 + 8;} else {num2 = 10 * num2 + 8;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button9_clicked (GtkButton *b) {
    numstr[i]='9';
    if (operatorSet == 0){num1 = 10 * num1 + 9;} else {num2 = 10 * num2 + 9;}
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_buttondel_clicked (GtkButton *b) {
    numstr[i]='/';
    operatorSet = 1;
    operation = 4;
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_buttonpower_clicked (GtkButton *b) {
    numstr[i]='*';
    operatorSet = 1;
    operation = 3;
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_buttonminus_clicked (GtkButton *b) {
    numstr[i]='-';
    operatorSet = 1;
    operation = 2;
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_buttonplus_clicked (GtkButton *b) {
    numstr[i]='+';
    operatorSet = 1;
    operation = 1;
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) numstr);
    i++;
}

void on_button10_clicked (GtkButton *b) {
    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) "");
    num1 = 0; num2 = 0; result = 0; operatorSet = 0;
    memset(numstr, 0, sizeof(numstr)); i = 0;
}

void on_buttonsame_clicked (GtkButton *b) {
    switch (operation)
    {
    case 1:
        result = num1 + num2;
        break;
    case 2:
        result = num1 - num2;
        break;
    case 3:
        result = num1 * num2;
        break;
    case 4:
        result = num1 / num2;
        break;
    default:
        break;
    }

    //if (operation == 1) {result = num1 + num2;}
    char str[100];
    printf("num1: %d\n", num1);
    printf("num2: %d\n", num2);
    printf("result: %d\n", result);
    sprintf(str, "%d\n", result);

    printf("numstr %s\n", numstr);

    gtk_label_set_text(GTK_LABEL(label1), (const gchar*) str);
    num1 = 0; num2 = 0; result = 0; operatorSet = 0;
    memset(numstr, 0, sizeof(numstr)); i = 0;
    printf("%s\n", numstr);
}

void on_buttonAddEvent_clicked(GtkButton *b){
    const char *newValue = gtk_entry_get_text(GTK_ENTRY(entry1));
    int rc = sqlite3_open("database.db", &db); //open file

    //write
    char *sql1 = "INSERT INTO Friends(Name) VALUES ('');";
    char sqlnew[100];
    int x = 35; //position of inserting
    strncpy(sqlnew, sql1, x);
    sqlnew[x] = '\0';
    strcat(sqlnew, newValue);
    strcat(sqlnew, sql1 + x);
    printf("\nsql command: %s\n", sqlnew);
    rc = sqlite3_exec(db, sqlnew, 0, 0, &err_msg);

    //read
    gtk_list_store_clear(GTK_LIST_STORE(store));
    char *sql= "SELECT * FROM Friends";
    rc = sqlite3_exec(db, sql, callback, store, &err_msg);
       
    sqlite3_close(db);
}

void on_buttonClearEvent_clicked(GtkButton *b){
    int rc = sqlite3_open("database.db", &db); //open file
    //Delete last row
    char *sql = "DELETE FROM Friends WHERE Id = (SELECT MAX(Id) FROM Friends);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    //read
    gtk_list_store_clear(GTK_LIST_STORE(store));
    sql= "SELECT * FROM Friends";
    rc = sqlite3_exec(db, sql, callback, store, &err_msg);
}

