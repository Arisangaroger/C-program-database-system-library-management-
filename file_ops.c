#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stddef.h>
#include "file_ops.h"

// Ensure data directory exists
void ensure_data_directory() {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        #ifdef _WIN32
        system("mkdir data");
        #else
        mkdir("data", 0700);
        #endif
    }
}

// Check if file exists
int file_exists(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Get next available ID for a file
int get_next_id(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return 1;
    
    int max_id = 0;
    void* buffer = malloc(1024); // Temporary buffer for reading records
    size_t record_size;
    
    // Determine record size based on file type
    if (strstr(filename, "authors.dat")) record_size = sizeof(Author);
    else if (strstr(filename, "publishers.dat")) record_size = sizeof(Publisher);
    else if (strstr(filename, "books.dat")) record_size = sizeof(Book);
    else if (strstr(filename, "members.dat")) record_size = sizeof(Member);
    else if (strstr(filename, "staff.dat")) record_size = sizeof(Staff);
    else if (strstr(filename, "borrowings.dat")) record_size = sizeof(Borrowing);
    else if (strstr(filename, "fines.dat")) record_size = sizeof(Fine);
    else record_size = 0;
    
    while (fread(buffer, record_size, 1, file) == 1) {
        int id;
        if (strstr(filename, "authors.dat")) id = ((Author*)buffer)->author_id;
        else if (strstr(filename, "publishers.dat")) id = ((Publisher*)buffer)->publisher_id;
        else if (strstr(filename, "books.dat")) id = ((Book*)buffer)->book_id;
        else if (strstr(filename, "members.dat")) id = ((Member*)buffer)->member_id;
        else if (strstr(filename, "staff.dat")) id = ((Staff*)buffer)->staff_id;
        else if (strstr(filename, "borrowings.dat")) id = ((Borrowing*)buffer)->borrowing_id;
        else if (strstr(filename, "fines.dat")) id = ((Fine*)buffer)->fine_id;
        else id = 0;
        
        if (id > max_id) max_id = id;
    }
    
    free(buffer);
    fclose(file);
    return max_id + 1;
}

// Generic function to add a record
int add_record(const char* filename, void* record, size_t size) {
    ensure_data_directory();
    FILE* file = fopen(filename, "ab");
    if (!file) return 0;
    
    int result = fwrite(record, size, 1, file);
    fclose(file);
    return result;
}

// Generic function to update a record
int update_record(const char* filename, void* record, size_t size, int id, int id_offset) {
    FILE* file = fopen(filename, "rb+");
    if (!file) return 0;

    void* buffer = malloc(size);
    int found = 0;
    long pos = 0;

    while (fread(buffer, size, 1, file) == 1) {
        int current_id;
        memcpy(&current_id, (char*)buffer + id_offset, sizeof(int));
        
        if (current_id == id) {
            fseek(file, pos, SEEK_SET);
            fwrite(record, size, 1, file);
            found = 1;
            break;
        }
        pos = ftell(file);
    }

    free(buffer);
    fclose(file);
    return found;
}

// Generic function to delete a record
int delete_record(const char* filename, int id, size_t size, int id_offset) {
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;

    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) {
        fclose(file);
        return 0;
    }

    void* buffer = malloc(size);
    int found = 0;

    while (fread(buffer, size, 1, file) == 1) {
        int current_id;
        memcpy(&current_id, (char*)buffer + id_offset, sizeof(int));
        
        if (current_id != id) {
            fwrite(buffer, size, 1, temp);
        } else {
            found = 1;
        }
    }

    free(buffer);
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.dat", filename);
    return found;
}

// Author operations
int add_author(Author* author) {
    author->author_id = get_next_id(AUTHORS_FILE);
    return add_record(AUTHORS_FILE, author, sizeof(Author));
}

int update_author(Author* author) {
    return update_record(AUTHORS_FILE, author, sizeof(Author), author->author_id, offsetof(Author, author_id));
}

int delete_author(int author_id) {
    return delete_record(AUTHORS_FILE, author_id, sizeof(Author), offsetof(Author, author_id));
}

Author* get_author(int author_id) {
    FILE* file = fopen(AUTHORS_FILE, "rb");
    if (!file) return NULL;

    Author* author = malloc(sizeof(Author));
    while (fread(author, sizeof(Author), 1, file) == 1) {
        if (author->author_id == author_id) {
            fclose(file);
            return author;
        }
    }

    free(author);
    fclose(file);
    return NULL;
}

Author** get_all_authors(int* count) {
    FILE* file = fopen(AUTHORS_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    // Count records
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Author);
    fseek(file, 0, SEEK_SET);

    Author** authors = malloc(*count * sizeof(Author*));
    for (int i = 0; i < *count; i++) {
        authors[i] = malloc(sizeof(Author));
        fread(authors[i], sizeof(Author), 1, file);
    }

    fclose(file);
    return authors;
}

Author* get_author_by_id(int author_id) {
    int count;
    Author** authors = get_all_authors(&count);
    if (authors) {
        for (int i = 0; i < count; i++) {
            if (authors[i]->author_id == author_id) {
                Author* author = (Author*)malloc(sizeof(Author));
                if (author) {
                    author->author_id = authors[i]->author_id;
                    strncpy(author->name, authors[i]->name, MAX_STRING_LENGTH - 1);
                    strncpy(author->bio, authors[i]->bio, MAX_STRING_LENGTH - 1);
                }
                // Free the array
                for (int j = 0; j < count; j++) {
                    free(authors[j]);
                }
                free(authors);
                return author;
            }
        }
        // Free the array if author not found
        for (int i = 0; i < count; i++) {
            free(authors[i]);
        }
        free(authors);
    }
    return NULL;
}

// Publisher operations
int add_publisher(Publisher* publisher) {
    publisher->publisher_id = get_next_id(PUBLISHERS_FILE);
    return add_record(PUBLISHERS_FILE, publisher, sizeof(Publisher));
}

int update_publisher(Publisher* publisher) {
    return update_record(PUBLISHERS_FILE, publisher, sizeof(Publisher), publisher->publisher_id, offsetof(Publisher, publisher_id));
}

int delete_publisher(int publisher_id) {
    return delete_record(PUBLISHERS_FILE, publisher_id, sizeof(Publisher), offsetof(Publisher, publisher_id));
}

Publisher* get_publisher(int publisher_id) {
    FILE* file = fopen(PUBLISHERS_FILE, "rb");
    if (!file) return NULL;

    Publisher* publisher = malloc(sizeof(Publisher));
    while (fread(publisher, sizeof(Publisher), 1, file) == 1) {
        if (publisher->publisher_id == publisher_id) {
            fclose(file);
            return publisher;
        }
    }

    free(publisher);
    fclose(file);
    return NULL;
}

Publisher* get_publisher_by_id(int publisher_id) {
    int count;
    Publisher** publishers = get_all_publishers(&count);
    if (publishers) {
        for (int i = 0; i < count; i++) {
            if (publishers[i]->publisher_id == publisher_id) {
                Publisher* publisher = (Publisher*)malloc(sizeof(Publisher));
                if (publisher) {
                    publisher->publisher_id = publishers[i]->publisher_id;
                    strncpy(publisher->name, publishers[i]->name, MAX_STRING_LENGTH - 1);
                    strncpy(publisher->address, publishers[i]->address, MAX_STRING_LENGTH - 1);
                    strncpy(publisher->phone, publishers[i]->phone, MAX_STRING_LENGTH - 1);
                }
                // Free the array
                for (int j = 0; j < count; j++) {
                    free(publishers[j]);
                }
                free(publishers);
                return publisher;
            }
        }
        // Free the array if publisher not found
        for (int i = 0; i < count; i++) {
            free(publishers[i]);
        }
        free(publishers);
    }
    return NULL;
}

Publisher** get_all_publishers(int* count) {
    FILE* file = fopen(PUBLISHERS_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Publisher);
    fseek(file, 0, SEEK_SET);

    Publisher** publishers = malloc(*count * sizeof(Publisher*));
    for (int i = 0; i < *count; i++) {
        publishers[i] = malloc(sizeof(Publisher));
        fread(publishers[i], sizeof(Publisher), 1, file);
    }

    fclose(file);
    return publishers;
}

// Book operations
int add_book(Book* book) {
    book->book_id = get_next_id(BOOKS_FILE);
    return add_record(BOOKS_FILE, book, sizeof(Book));
}

int update_book(Book* book) {
    return update_record(BOOKS_FILE, book, sizeof(Book), book->book_id, offsetof(Book, book_id));
}

int delete_book(int book_id) {
    return delete_record(BOOKS_FILE, book_id, sizeof(Book), offsetof(Book, book_id));
}

Book* get_book(int book_id) {
    FILE* file = fopen(BOOKS_FILE, "rb");
    if (!file) return NULL;

    Book* book = malloc(sizeof(Book));
    while (fread(book, sizeof(Book), 1, file) == 1) {
        if (book->book_id == book_id) {
            fclose(file);
            return book;
        }
    }

    free(book);
    fclose(file);
    return NULL;
}

Book** get_all_books(int* count) {
    FILE* file = fopen(BOOKS_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Book);
    fseek(file, 0, SEEK_SET);

    Book** books = malloc(*count * sizeof(Book*));
    for (int i = 0; i < *count; i++) {
        books[i] = malloc(sizeof(Book));
        fread(books[i], sizeof(Book), 1, file);
    }

    fclose(file);
    return books;
}

// Member operations
int add_member(Member* member) {
    member->member_id = get_next_id(MEMBERS_FILE);
    return add_record(MEMBERS_FILE, member, sizeof(Member));
}

int update_member(Member* member) {
    return update_record(MEMBERS_FILE, member, sizeof(Member), member->member_id, offsetof(Member, member_id));
}

int delete_member(int member_id) {
    return delete_record(MEMBERS_FILE, member_id, sizeof(Member), offsetof(Member, member_id));
}

Member* get_member(int member_id) {
    FILE* file = fopen(MEMBERS_FILE, "rb");
    if (!file) return NULL;

    Member* member = malloc(sizeof(Member));
    while (fread(member, sizeof(Member), 1, file) == 1) {
        if (member->member_id == member_id) {
            fclose(file);
            return member;
        }
    }

    free(member);
    fclose(file);
    return NULL;
}

Member** get_all_members(int* count) {
    FILE* file = fopen(MEMBERS_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Member);
    fseek(file, 0, SEEK_SET);

    Member** members = malloc(*count * sizeof(Member*));
    for (int i = 0; i < *count; i++) {
        members[i] = malloc(sizeof(Member));
        fread(members[i], sizeof(Member), 1, file);
    }

    fclose(file);
    return members;
}

// Staff operations
int add_staff(Staff* staff) {
    staff->staff_id = get_next_id(STAFF_FILE);
    return add_record(STAFF_FILE, staff, sizeof(Staff));
}

int update_staff(Staff* staff) {
    return update_record(STAFF_FILE, staff, sizeof(Staff), staff->staff_id, offsetof(Staff, staff_id));
}

int delete_staff(int staff_id) {
    return delete_record(STAFF_FILE, staff_id, sizeof(Staff), offsetof(Staff, staff_id));
}

Staff* get_staff(int staff_id) {
    FILE* file = fopen(STAFF_FILE, "rb");
    if (!file) return NULL;

    Staff* staff = malloc(sizeof(Staff));
    while (fread(staff, sizeof(Staff), 1, file) == 1) {
        if (staff->staff_id == staff_id) {
            fclose(file);
            return staff;
        }
    }

    free(staff);
    fclose(file);
    return NULL;
}

Staff** get_all_staff(int* count) {
    FILE* file = fopen(STAFF_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Staff);
    fseek(file, 0, SEEK_SET);

    Staff** staff = malloc(*count * sizeof(Staff*));
    for (int i = 0; i < *count; i++) {
        staff[i] = malloc(sizeof(Staff));
        fread(staff[i], sizeof(Staff), 1, file);
    }

    fclose(file);
    return staff;
}

// Borrowing operations
int add_borrowing(Borrowing* borrowing) {
    borrowing->borrowing_id = get_next_id(BORROWINGS_FILE);
    return add_record(BORROWINGS_FILE, borrowing, sizeof(Borrowing));
}

int update_borrowing(Borrowing* borrowing) {
    return update_record(BORROWINGS_FILE, borrowing, sizeof(Borrowing), borrowing->borrowing_id, offsetof(Borrowing, borrowing_id));
}

int delete_borrowing(int borrowing_id) {
    return delete_record(BORROWINGS_FILE, borrowing_id, sizeof(Borrowing), offsetof(Borrowing, borrowing_id));
}

Borrowing* get_borrowing(int borrowing_id) {
    FILE* file = fopen(BORROWINGS_FILE, "rb");
    if (!file) return NULL;

    Borrowing* borrowing = malloc(sizeof(Borrowing));
    while (fread(borrowing, sizeof(Borrowing), 1, file) == 1) {
        if (borrowing->borrowing_id == borrowing_id) {
            fclose(file);
            return borrowing;
        }
    }

    free(borrowing);
    fclose(file);
    return NULL;
}

Borrowing** get_all_borrowings(int* count) {
    FILE* file = fopen(BORROWINGS_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Borrowing);
    fseek(file, 0, SEEK_SET);

    Borrowing** borrowings = malloc(*count * sizeof(Borrowing*));
    for (int i = 0; i < *count; i++) {
        borrowings[i] = malloc(sizeof(Borrowing));
        fread(borrowings[i], sizeof(Borrowing), 1, file);
    }

    fclose(file);
    return borrowings;
}

// Fine operations
int add_fine(Fine* fine) {
    fine->fine_id = get_next_id(FINES_FILE);
    return add_record(FINES_FILE, fine, sizeof(Fine));
}

int update_fine(Fine* fine) {
    return update_record(FINES_FILE, fine, sizeof(Fine), fine->fine_id, offsetof(Fine, fine_id));
}

int delete_fine(int fine_id) {
    return delete_record(FINES_FILE, fine_id, sizeof(Fine), offsetof(Fine, fine_id));
}

Fine* get_fine(int fine_id) {
    FILE* file = fopen(FINES_FILE, "rb");
    if (!file) return NULL;

    Fine* fine = malloc(sizeof(Fine));
    while (fread(fine, sizeof(Fine), 1, file) == 1) {
        if (fine->fine_id == fine_id) {
            fclose(file);
            return fine;
        }
    }

    free(fine);
    fclose(file);
    return NULL;
}

Fine** get_all_fines(int* count) {
    FILE* file = fopen(FINES_FILE, "rb");
    if (!file) {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    *count = size / sizeof(Fine);
    fseek(file, 0, SEEK_SET);

    Fine** fines = malloc(*count * sizeof(Fine*));
    for (int i = 0; i < *count; i++) {
        fines[i] = malloc(sizeof(Fine));
        fread(fines[i], sizeof(Fine), 1, file);
    }

    fclose(file);
    return fines;
}

// Function to get a book by its ID
Book* get_book_by_id(int book_id) {
    int count;
    Book** books = get_all_books(&count);
    
    if (books) {
        for (int i = 0; i < count; i++) {
            if (books[i]->book_id == book_id) {
                // Create a copy of the book to return
                Book* book_copy = (Book*)malloc(sizeof(Book));
                if (book_copy) {
                    memcpy(book_copy, books[i], sizeof(Book));
                    // Free the array of books
                    for (int j = 0; j < count; j++) {
                        free(books[j]);
                    }
                    free(books);
                    return book_copy;
                }
            }
        }
        // Free the array of books if book not found
        for (int i = 0; i < count; i++) {
            free(books[i]);
        }
        free(books);
    }
    return NULL;
}

// Note: For brevity, I'm not showing all implementations, but they would follow the same pattern
// as the Author operations, just with their respective structures and file paths. 