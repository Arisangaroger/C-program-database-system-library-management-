#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "models.h"

// File paths
#define AUTHORS_FILE "data/authors.dat"
#define PUBLISHERS_FILE "data/publishers.dat"
#define BOOKS_FILE "data/books.dat"
#define MEMBERS_FILE "data/members.dat"
#define STAFF_FILE "data/staff.dat"
#define BORROWINGS_FILE "data/borrowings.dat"
#define FINES_FILE "data/fines.dat"

// Generic file operations
int get_next_id(const char* filename);
int file_exists(const char* filename);
void ensure_data_directory();

// Author operations
Author* get_author_by_id(int author_id);
int add_author(Author* author);
int update_author(Author* author);
int delete_author(int author_id);
Author** get_all_authors(int* count);

// Publisher operations
Publisher* get_publisher_by_id(int publisher_id);
int add_publisher(Publisher* publisher);
int update_publisher(Publisher* publisher);
int delete_publisher(int publisher_id);
Publisher** get_all_publishers(int* count);

// Book operations
Book* get_book_by_id(int book_id);
int add_book(Book* book);
int update_book(Book* book);
int delete_book(int book_id);
Book** get_all_books(int* count);
Book** get_books_by_author(int author_id, int* count);
Book** get_books_by_publisher(int publisher_id, int* count);

// Member operations
int add_member(Member* member);
int update_member(Member* member);
int delete_member(int member_id);
Member* get_member(int member_id);
Member** get_all_members(int* count);

// Staff operations
int add_staff(Staff* staff);
int update_staff(Staff* staff);
int delete_staff(int staff_id);
Staff* get_staff(int staff_id);
Staff** get_all_staff(int* count);

// Borrowing operations
int add_borrowing(Borrowing* borrowing);
int update_borrowing(Borrowing* borrowing);
int delete_borrowing(int borrowing_id);
Borrowing* get_borrowing(int borrowing_id);
Borrowing** get_all_borrowings(int* count);
Borrowing** get_member_borrowings(int member_id, int* count);
Borrowing** get_active_borrowings(int* count);

// Fine operations
int add_fine(Fine* fine);
int update_fine(Fine* fine);
int delete_fine(int fine_id);
Fine* get_fine(int fine_id);
Fine** get_all_fines(int* count);
Fine** get_member_fines(int member_id, int* count);
Fine** get_unpaid_fines(int* count);

// Function declarations
void initialize_data(void);
void cleanup_data(void);

#endif // FILE_OPS_H 