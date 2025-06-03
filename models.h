#ifndef MODELS_H
#define MODELS_H

#include <time.h>

#define MAX_STRING_LENGTH 256
#define MAX_BIO_LENGTH 1024
#define DATE_FORMAT "%Y-%m-%d"

// Author structure
typedef struct {
    int author_id;
    char name[MAX_STRING_LENGTH];
    char bio[MAX_BIO_LENGTH];
} Author;

// Publisher structure
typedef struct {
    int publisher_id;
    char name[MAX_STRING_LENGTH];
    char address[MAX_STRING_LENGTH];
    char phone[MAX_STRING_LENGTH];
} Publisher;

// Book structure
typedef struct {
    int book_id;
    char title[MAX_STRING_LENGTH];
    int author_id;
    int publisher_id;
    char isbn[MAX_STRING_LENGTH];
    char genre[MAX_STRING_LENGTH];
    int year_published;
    int copies_available;
    char shelf_location[MAX_STRING_LENGTH];
} Book;

// Member structure
typedef struct {
    int member_id;
    char name[MAX_STRING_LENGTH];
    char address[MAX_STRING_LENGTH];
    char phone[MAX_STRING_LENGTH];
    char email[MAX_STRING_LENGTH];
    char date_joined[11];  // YYYY-MM-DD + null terminator
    char membership_status[MAX_STRING_LENGTH];
} Member;

// Staff structure
typedef struct {
    int staff_id;
    char name[MAX_STRING_LENGTH];
    char role[MAX_STRING_LENGTH];
    char email[MAX_STRING_LENGTH];
    char phone[MAX_STRING_LENGTH];
} Staff;

// Borrowing structure
typedef struct {
    int borrowing_id;
    int book_id;
    int member_id;
    char borrow_date[11];  // YYYY-MM-DD + null terminator
    char due_date[11];     // YYYY-MM-DD + null terminator
    char return_date[11];  // YYYY-MM-DD + null terminator
    int staff_id;
} Borrowing;

// Fine structure
typedef struct {
    int fine_id;
    int borrowing_id;
    double amount;
    int paid;
    char date_paid[11];  // YYYY-MM-DD + null terminator
} Fine;

#endif // MODELS_H 