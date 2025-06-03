#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "models.h"

// Main menu functions
void show_main_menu();
void handle_main_menu_choice(int choice);

// Book management
void show_book_menu();
void handle_book_menu_choice(int choice);
void add_book_ui();
void update_book_ui();
void delete_book_ui();
void list_books_ui();
void search_book_ui();

// Author management
void show_author_menu();
void handle_author_menu_choice(int choice);
void add_author_ui();
void update_author_ui();
void delete_author_ui();
void list_authors_ui();
void search_author_ui();

// Publisher management
void show_publisher_menu();
void handle_publisher_menu_choice(int choice);
void add_publisher_ui();
void update_publisher_ui();
void delete_publisher_ui();
void list_publishers_ui();
void search_publisher_ui();

// Member management
void show_member_menu();
void handle_member_menu_choice(int choice);
void add_member_ui();
void update_member_ui();
void delete_member_ui();
void list_members_ui();
void search_member_ui();

// Staff management
void show_staff_menu();
void handle_staff_menu_choice(int choice);
void add_staff_ui();
void update_staff_ui();
void delete_staff_ui();
void list_staff_ui();
void search_staff_ui();

// Borrowing management
void show_borrowing_menu();
void handle_borrowing_menu_choice(int choice);
void add_borrowing_ui();
void update_borrowing_ui();
void delete_borrowing_ui();
void list_borrowings_ui();
void search_borrowing_ui();
void return_book_ui();

// Fine management
void show_fine_menu();
void handle_fine_menu_choice(int choice);
void add_fine_ui();
void update_fine_ui();
void delete_fine_ui();
void list_fines_ui();
void search_fine_ui();
void pay_fine_ui();

// Utility functions
void clear_screen();
void print_header(const char* title);
void print_footer();
void wait_for_enter();
int get_int_input(const char* prompt);
double get_double_input(const char* prompt);
void get_string_input(const char* prompt, char* buffer, int max_length);
void get_date_input(const char* prompt, char* buffer);

#endif // CONSOLE_UI_H 