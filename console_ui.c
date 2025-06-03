#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "console_ui.h"
#include "file_ops.h"

// Utility functions
void clear_screen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void print_header(const char* title) {
    clear_screen();
    printf("\n=== %s ===\n\n", title);
}

void print_footer() {
    printf("\nPress Enter to continue...");
    getchar();
    while (getchar() != '\n');
}

void wait_for_enter() {
    printf("\nPress Enter to continue...");
    getchar();
    while (getchar() != '\n');
}

int get_int_input(const char* prompt) {
    int value;
    char input[32];
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            if (sscanf(input, "%d", &value) == 1) {
                return value;
            }
        }
        printf("Invalid input. Please enter a number.\n");
    }
}

double get_double_input(const char* prompt) {
    double value;
    char input[32];
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            if (sscanf(input, "%lf", &value) == 1) {
                return value;
            }
        }
        printf("Invalid input. Please enter a number.\n");
    }
}

void get_string_input(const char* prompt, char* buffer, int max_length) {
    printf("%s", prompt);
    if (fgets(buffer, max_length, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

void get_date_input(const char* prompt, char* buffer) {
    while (1) {
        printf("%s (YYYY-MM-DD): ", prompt);
        if (fgets(buffer, 11, stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            // Basic date format validation
            if (strlen(buffer) == 10 && 
                isdigit(buffer[0]) && isdigit(buffer[1]) && 
                isdigit(buffer[2]) && isdigit(buffer[3]) && 
                buffer[4] == '-' && 
                isdigit(buffer[5]) && isdigit(buffer[6]) && 
                buffer[7] == '-' && 
                isdigit(buffer[8]) && isdigit(buffer[9])) {
                return;
            }
        }
        printf("Invalid date format. Please use YYYY-MM-DD.\n");
    }
}

// Main menu
void show_main_menu() {
    print_header("Library Management System");
    printf("1. Book Management\n");
    printf("2. Author Management\n");
    printf("3. Publisher Management\n");
    printf("4. Member Management\n");
    printf("5. Staff Management\n");
    printf("6. Borrowing Management\n");
    printf("7. Fine Management\n");
    printf("0. Exit\n");
    printf("\nEnter your choice: ");
}

void handle_main_menu_choice(int choice) {
    int subchoice;
    switch (choice) {
        case 1:
            while (1) {
                show_book_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_book_menu_choice(subchoice);
            }
            break;
        case 2:
            while (1) {
                show_author_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_author_menu_choice(subchoice);
            }
            break;
        case 3:
            while (1) {
                show_publisher_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_publisher_menu_choice(subchoice);
            }
            break;
        case 4:
            while (1) {
                show_member_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_member_menu_choice(subchoice);
            }
            break;
        case 5:
            while (1) {
                show_staff_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_staff_menu_choice(subchoice);
            }
            break;
        case 6:
            while (1) {
                show_borrowing_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_borrowing_menu_choice(subchoice);
            }
            break;
        case 7:
            while (1) {
                show_fine_menu();
                subchoice = get_int_input("");
                if (subchoice == 0) break;
                handle_fine_menu_choice(subchoice);
            }
            break;
        case 0:
            printf("\nThank you for using the Library Management System!\n");
            exit(0);
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Book management
void show_book_menu() {
    print_header("Book Management");
    printf("1. Add New Book\n");
    printf("2. Update Book\n");
    printf("3. Delete Book\n");
    printf("4. List All Books\n");
    printf("5. Search Book\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_book_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_book_ui();
            break;
        case 2:
            update_book_ui();
            break;
        case 3:
            delete_book_ui();
            break;
        case 4:
            list_books_ui();
            break;
        case 5:
            search_book_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

void add_book_ui() {
    print_header("Add New Book");
    
    Book book;
    memset(&book, 0, sizeof(Book));
    
    get_string_input("Enter book title: ", book.title, MAX_STRING_LENGTH);
    book.author_id = get_int_input("Enter author ID: ");
    book.publisher_id = get_int_input("Enter publisher ID: ");
    get_string_input("Enter ISBN: ", book.isbn, MAX_STRING_LENGTH);
    get_string_input("Enter genre: ", book.genre, MAX_STRING_LENGTH);
    book.year_published = get_int_input("Enter year published: ");
    book.copies_available = get_int_input("Enter number of copies available: ");
    get_string_input("Enter shelf location: ", book.shelf_location, MAX_STRING_LENGTH);
    
    if (add_book(&book)) {
        printf("\nBook added successfully!\n");
    } else {
        printf("\nFailed to add book.\n");
    }
    
    wait_for_enter();
}

void update_book_ui() {
    print_header("Update Book");
    
    int book_id = get_int_input("Enter book ID to update: ");
    Book* book = get_book(book_id);
    
    if (!book) {
        printf("\nBook not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nCurrent book details:\n");
    printf("Title: %s\n", book->title);
    printf("Author ID: %d\n", book->author_id);
    printf("Publisher ID: %d\n", book->publisher_id);
    printf("ISBN: %s\n", book->isbn);
    printf("Genre: %s\n", book->genre);
    printf("Year Published: %d\n", book->year_published);
    printf("Copies Available: %d\n", book->copies_available);
    printf("Shelf Location: %s\n", book->shelf_location);
    
    printf("\nEnter new details (press Enter to keep current value):\n");
    
    char input[MAX_STRING_LENGTH];
    get_string_input("Enter new title: ", input, MAX_STRING_LENGTH);
    if (strlen(input) > 0) strcpy(book->title, input);
    
    int author_id = get_int_input("Enter new author ID (0 to keep current): ");
    if (author_id > 0) book->author_id = author_id;
    
    int publisher_id = get_int_input("Enter new publisher ID (0 to keep current): ");
    if (publisher_id > 0) book->publisher_id = publisher_id;
    
    get_string_input("Enter new ISBN: ", input, MAX_STRING_LENGTH);
    if (strlen(input) > 0) strcpy(book->isbn, input);
    
    get_string_input("Enter new genre: ", input, MAX_STRING_LENGTH);
    if (strlen(input) > 0) strcpy(book->genre, input);
    
    int year = get_int_input("Enter new year published (0 to keep current): ");
    if (year > 0) book->year_published = year;
    
    int copies = get_int_input("Enter new number of copies available (0 to keep current): ");
    if (copies >= 0) book->copies_available = copies;
    
    get_string_input("Enter new shelf location: ", input, MAX_STRING_LENGTH);
    if (strlen(input) > 0) strcpy(book->shelf_location, input);
    
    if (update_book(book)) {
        printf("\nBook updated successfully!\n");
    } else {
        printf("\nFailed to update book.\n");
    }
    
    free(book);
    wait_for_enter();
}

void delete_book_ui() {
    print_header("Delete Book");
    
    int book_id = get_int_input("Enter book ID to delete: ");
    
    if (delete_book(book_id)) {
        printf("\nBook deleted successfully!\n");
    } else {
        printf("\nFailed to delete book or book not found.\n");
    }
    
    wait_for_enter();
}

void list_books_ui() {
    print_header("List All Books");
    
    int count;
    Book** books = get_all_books(&count);
    
    if (!books || count == 0) {
        printf("\nNo books found in the system.\n");
    } else {
        printf("\nTotal books: %d\n\n", count);
        printf("%-5s %-30s %-15s %-15s %-10s %-15s %-10s %-15s\n",
               "ID", "Title", "Author ID", "Publisher ID", "ISBN", "Genre", "Year", "Copies");
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%-5d %-30s %-15d %-15d %-10s %-15s %-10d %-15d\n",
                   books[i]->book_id,
                   books[i]->title,
                   books[i]->author_id,
                   books[i]->publisher_id,
                   books[i]->isbn,
                   books[i]->genre,
                   books[i]->year_published,
                   books[i]->copies_available);
        }
        
        // Free allocated memory
        for (int i = 0; i < count; i++) {
            free(books[i]);
        }
        free(books);
    }
    
    wait_for_enter();
}

void search_book_ui() {
    print_header("Search Book");
    
    printf("Search by:\n");
    printf("1. Book ID\n");
    printf("2. Title\n");
    printf("3. ISBN\n");
    printf("0. Back\n");
    
    int choice = get_int_input("\nEnter your choice: ");
    
    if (choice == 0) return;
    
    int count;
    Book** books = NULL;
    char search_term[MAX_STRING_LENGTH];
    
    switch (choice) {
        case 1: {
            int book_id = get_int_input("Enter book ID: ");
            Book* book = get_book(book_id);
            if (book) {
                books = malloc(sizeof(Book*));
                books[0] = book;
                count = 1;
            } else {
                count = 0;
            }
            break;
        }
        case 2: {
            get_string_input("Enter book title: ", search_term, MAX_STRING_LENGTH);
            books = get_all_books(&count);
            if (books) {
                // Filter books by title
                int filtered_count = 0;
                for (int i = 0; i < count; i++) {
                    if (strstr(books[i]->title, search_term)) {
                        if (i != filtered_count) {
                            books[filtered_count] = books[i];
                        }
                        filtered_count++;
                    } else {
                        free(books[i]);
                    }
                }
                count = filtered_count;
            }
            break;
        }
        case 3: {
            get_string_input("Enter ISBN: ", search_term, MAX_STRING_LENGTH);
            books = get_all_books(&count);
            if (books) {
                // Filter books by ISBN
                int filtered_count = 0;
                for (int i = 0; i < count; i++) {
                    if (strstr(books[i]->isbn, search_term)) {
                        if (i != filtered_count) {
                            books[filtered_count] = books[i];
                        }
                        filtered_count++;
                    } else {
                        free(books[i]);
                    }
                }
                count = filtered_count;
            }
            break;
        }
        default:
            printf("\nInvalid choice.\n");
            wait_for_enter();
            return;
    }
    
    if (!books || count == 0) {
        printf("\nNo books found matching your search criteria.\n");
    } else {
        printf("\nFound %d book(s):\n\n", count);
        printf("%-5s %-30s %-15s %-15s %-10s %-15s %-10s %-15s\n",
               "ID", "Title", "Author ID", "Publisher ID", "ISBN", "Genre", "Year", "Copies");
        printf("----------------------------------------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%-5d %-30s %-15d %-15d %-10s %-15s %-10d %-15d\n",
                   books[i]->book_id,
                   books[i]->title,
                   books[i]->author_id,
                   books[i]->publisher_id,
                   books[i]->isbn,
                   books[i]->genre,
                   books[i]->year_published,
                   books[i]->copies_available);
        }
        
        // Free allocated memory
        for (int i = 0; i < count; i++) {
            free(books[i]);
        }
        free(books);
    }
    
    wait_for_enter();
}

// Author menu
void show_author_menu() {
    print_header("Author Management");
    printf("1. Add New Author\n");
    printf("2. Update Author\n");
    printf("3. Delete Author\n");
    printf("4. List All Authors\n");
    printf("5. Search Author\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_author_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_author_ui();
            break;
        case 2:
            update_author_ui();
            break;
        case 3:
            delete_author_ui();
            break;
        case 4:
            list_authors_ui();
            break;
        case 5:
            search_author_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Author UI functions
void add_author_ui() {
    print_header("Add New Author");
    
    Author author;
    memset(&author, 0, sizeof(Author));
    
    get_string_input("Enter author name: ", author.name, MAX_STRING_LENGTH);
    get_string_input("Enter author bio: ", author.bio, MAX_BIO_LENGTH);
    
    if (add_author(&author)) {
        printf("\nAuthor added successfully!\n");
    } else {
        printf("\nFailed to add author.\n");
    }
    
    wait_for_enter();
}

void update_author_ui() {
    print_header("Update Author");
    int author_id = get_int_input("Enter author ID to update: ");
    
    Author* author = get_author(author_id);
    if (!author) {
        printf("\nAuthor not found!\n");
        wait_for_enter();
        return;
    }
    
    get_string_input("Enter new name (or press Enter to keep current): ", author->name, MAX_STRING_LENGTH);
    get_string_input("Enter new bio (or press Enter to keep current): ", author->bio, MAX_BIO_LENGTH);
    
    if (update_author(author)) {
        printf("\nAuthor updated successfully!\n");
    } else {
        printf("\nFailed to update author.\n");
    }
    
    free(author);
    wait_for_enter();
}

void delete_author_ui() {
    print_header("Delete Author");
    int author_id = get_int_input("Enter author ID to delete: ");
    
    if (delete_author(author_id)) {
        printf("\nAuthor deleted successfully!\n");
    } else {
        printf("\nFailed to delete author.\n");
    }
    
    wait_for_enter();
}

void list_authors_ui() {
    print_header("List All Authors");
    
    int count;
    Author** authors = get_all_authors(&count);
    
    if (!authors) {
        printf("\nNo authors found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal authors: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Author ID: %d\n", authors[i]->author_id);
        printf("Name: %s\n", authors[i]->name);
        printf("Bio: %s\n", authors[i]->bio);
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(authors[i]);
    }
    free(authors);
    
    wait_for_enter();
}

void search_author_ui() {
    print_header("Search Author");
    int author_id = get_int_input("Enter author ID to search: ");
    
    Author* author = get_author(author_id);
    if (!author) {
        printf("\nAuthor not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nAuthor ID: %d\n", author->author_id);
    printf("Name: %s\n", author->name);
    printf("Bio: %s\n", author->bio);
    
    free(author);
    wait_for_enter();
}

// Publisher menu
void show_publisher_menu() {
    print_header("Publisher Management");
    printf("1. Add New Publisher\n");
    printf("2. Update Publisher\n");
    printf("3. Delete Publisher\n");
    printf("4. List All Publishers\n");
    printf("5. Search Publisher\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_publisher_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_publisher_ui();
            break;
        case 2:
            update_publisher_ui();
            break;
        case 3:
            delete_publisher_ui();
            break;
        case 4:
            list_publishers_ui();
            break;
        case 5:
            search_publisher_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Publisher UI functions
void add_publisher_ui() {
    print_header("Add New Publisher");
    
    Publisher publisher;
    memset(&publisher, 0, sizeof(Publisher));
    
    get_string_input("Enter publisher name: ", publisher.name, MAX_STRING_LENGTH);
    get_string_input("Enter publisher address: ", publisher.address, MAX_STRING_LENGTH);
    get_string_input("Enter publisher contact info: ", publisher.contact_info, MAX_STRING_LENGTH);
    
    if (add_publisher(&publisher)) {
        printf("\nPublisher added successfully!\n");
    } else {
        printf("\nFailed to add publisher.\n");
    }
    
    wait_for_enter();
}

void update_publisher_ui() {
    print_header("Update Publisher");
    int publisher_id = get_int_input("Enter publisher ID to update: ");
    
    Publisher* publisher = get_publisher(publisher_id);
    if (!publisher) {
        printf("\nPublisher not found!\n");
        wait_for_enter();
        return;
    }
    
    get_string_input("Enter new name (or press Enter to keep current): ", publisher->name, MAX_STRING_LENGTH);
    get_string_input("Enter new address (or press Enter to keep current): ", publisher->address, MAX_STRING_LENGTH);
    get_string_input("Enter new contact info (or press Enter to keep current): ", publisher->contact_info, MAX_STRING_LENGTH);
    
    if (update_publisher(publisher)) {
        printf("\nPublisher updated successfully!\n");
    } else {
        printf("\nFailed to update publisher.\n");
    }
    
    free(publisher);
    wait_for_enter();
}

void delete_publisher_ui() {
    print_header("Delete Publisher");
    int publisher_id = get_int_input("Enter publisher ID to delete: ");
    
    if (delete_publisher(publisher_id)) {
        printf("\nPublisher deleted successfully!\n");
    } else {
        printf("\nFailed to delete publisher.\n");
    }
    
    wait_for_enter();
}

void list_publishers_ui() {
    print_header("List All Publishers");
    
    int count;
    Publisher** publishers = get_all_publishers(&count);
    
    if (!publishers) {
        printf("\nNo publishers found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal publishers: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Publisher ID: %d\n", publishers[i]->publisher_id);
        printf("Name: %s\n", publishers[i]->name);
        printf("Address: %s\n", publishers[i]->address);
        printf("Contact Info: %s\n", publishers[i]->contact_info);
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(publishers[i]);
    }
    free(publishers);
    
    wait_for_enter();
}

void search_publisher_ui() {
    print_header("Search Publisher");
    int publisher_id = get_int_input("Enter publisher ID to search: ");
    
    Publisher* publisher = get_publisher(publisher_id);
    if (!publisher) {
        printf("\nPublisher not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nPublisher ID: %d\n", publisher->publisher_id);
    printf("Name: %s\n", publisher->name);
    printf("Address: %s\n", publisher->address);
    printf("Contact Info: %s\n", publisher->contact_info);
    
    free(publisher);
    wait_for_enter();
}

// Member menu
void show_member_menu() {
    print_header("Member Management");
    printf("1. Add New Member\n");
    printf("2. Update Member\n");
    printf("3. Delete Member\n");
    printf("4. List All Members\n");
    printf("5. Search Member\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_member_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_member_ui();
            break;
        case 2:
            update_member_ui();
            break;
        case 3:
            delete_member_ui();
            break;
        case 4:
            list_members_ui();
            break;
        case 5:
            search_member_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Member UI functions
void add_member_ui() {
    print_header("Add New Member");
    
    Member member;
    memset(&member, 0, sizeof(Member));
    
    get_string_input("Enter member name: ", member.name, MAX_STRING_LENGTH);
    get_string_input("Enter member address: ", member.address, MAX_STRING_LENGTH);
    get_string_input("Enter member phone: ", member.phone, MAX_STRING_LENGTH);
    get_string_input("Enter member email: ", member.email, MAX_STRING_LENGTH);
    get_date_input("Enter date joined", member.date_joined);
    get_string_input("Enter membership status: ", member.membership_status, MAX_STRING_LENGTH);
    
    if (add_member(&member)) {
        printf("\nMember added successfully!\n");
    } else {
        printf("\nFailed to add member.\n");
    }
    
    wait_for_enter();
}

void update_member_ui() {
    print_header("Update Member");
    int member_id = get_int_input("Enter member ID to update: ");
    
    Member* member = get_member(member_id);
    if (!member) {
        printf("\nMember not found!\n");
        wait_for_enter();
        return;
    }
    
    get_string_input("Enter new name (or press Enter to keep current): ", member->name, MAX_STRING_LENGTH);
    get_string_input("Enter new address (or press Enter to keep current): ", member->address, MAX_STRING_LENGTH);
    get_string_input("Enter new phone (or press Enter to keep current): ", member->phone, MAX_STRING_LENGTH);
    get_string_input("Enter new email (or press Enter to keep current): ", member->email, MAX_STRING_LENGTH);
    get_date_input("Enter new date joined (or press Enter to keep current)", member->date_joined);
    get_string_input("Enter new membership status (or press Enter to keep current): ", member->membership_status, MAX_STRING_LENGTH);
    
    if (update_member(member)) {
        printf("\nMember updated successfully!\n");
    } else {
        printf("\nFailed to update member.\n");
    }
    
    free(member);
    wait_for_enter();
}

void delete_member_ui() {
    print_header("Delete Member");
    int member_id = get_int_input("Enter member ID to delete: ");
    
    if (delete_member(member_id)) {
        printf("\nMember deleted successfully!\n");
    } else {
        printf("\nFailed to delete member.\n");
    }
    
    wait_for_enter();
}

void list_members_ui() {
    print_header("List All Members");
    
    int count;
    Member** members = get_all_members(&count);
    
    if (!members) {
        printf("\nNo members found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal members: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Member ID: %d\n", members[i]->member_id);
        printf("Name: %s\n", members[i]->name);
        printf("Address: %s\n", members[i]->address);
        printf("Phone: %s\n", members[i]->phone);
        printf("Email: %s\n", members[i]->email);
        printf("Date Joined: %s\n", members[i]->date_joined);
        printf("Membership Status: %s\n", members[i]->membership_status);
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(members[i]);
    }
    free(members);
    
    wait_for_enter();
}

void search_member_ui() {
    print_header("Search Member");
    int member_id = get_int_input("Enter member ID to search: ");
    
    Member* member = get_member(member_id);
    if (!member) {
        printf("\nMember not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nMember ID: %d\n", member->member_id);
    printf("Name: %s\n", member->name);
    printf("Address: %s\n", member->address);
    printf("Phone: %s\n", member->phone);
    printf("Email: %s\n", member->email);
    printf("Date Joined: %s\n", member->date_joined);
    printf("Membership Status: %s\n", member->membership_status);
    
    free(member);
    wait_for_enter();
}

// Staff menu
void show_staff_menu() {
    print_header("Staff Management");
    printf("1. Add New Staff\n");
    printf("2. Update Staff\n");
    printf("3. Delete Staff\n");
    printf("4. List All Staff\n");
    printf("5. Search Staff\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_staff_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_staff_ui();
            break;
        case 2:
            update_staff_ui();
            break;
        case 3:
            delete_staff_ui();
            break;
        case 4:
            list_staff_ui();
            break;
        case 5:
            search_staff_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Staff UI functions
void add_staff_ui() {
    print_header("Add New Staff");
    
    Staff staff;
    memset(&staff, 0, sizeof(Staff));
    
    get_string_input("Enter staff name: ", staff.name, MAX_STRING_LENGTH);
    get_string_input("Enter staff role: ", staff.role, MAX_STRING_LENGTH);
    get_string_input("Enter staff email: ", staff.email, MAX_STRING_LENGTH);
    get_string_input("Enter staff phone: ", staff.phone, MAX_STRING_LENGTH);
    
    if (add_staff(&staff)) {
        printf("\nStaff added successfully!\n");
    } else {
        printf("\nFailed to add staff.\n");
    }
    
    wait_for_enter();
}

void update_staff_ui() {
    print_header("Update Staff");
    int staff_id = get_int_input("Enter staff ID to update: ");
    
    Staff* staff = get_staff(staff_id);
    if (!staff) {
        printf("\nStaff not found!\n");
        wait_for_enter();
        return;
    }
    
    get_string_input("Enter new name (or press Enter to keep current): ", staff->name, MAX_STRING_LENGTH);
    get_string_input("Enter new role (or press Enter to keep current): ", staff->role, MAX_STRING_LENGTH);
    get_string_input("Enter new email (or press Enter to keep current): ", staff->email, MAX_STRING_LENGTH);
    get_string_input("Enter new phone (or press Enter to keep current): ", staff->phone, MAX_STRING_LENGTH);
    
    if (update_staff(staff)) {
        printf("\nStaff updated successfully!\n");
    } else {
        printf("\nFailed to update staff.\n");
    }
    
    free(staff);
    wait_for_enter();
}

void delete_staff_ui() {
    print_header("Delete Staff");
    int staff_id = get_int_input("Enter staff ID to delete: ");
    
    if (delete_staff(staff_id)) {
        printf("\nStaff deleted successfully!\n");
    } else {
        printf("\nFailed to delete staff.\n");
    }
    
    wait_for_enter();
}

void list_staff_ui() {
    print_header("List All Staff");
    
    int count;
    Staff** staff = get_all_staff(&count);
    
    if (!staff) {
        printf("\nNo staff found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal staff: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Staff ID: %d\n", staff[i]->staff_id);
        printf("Name: %s\n", staff[i]->name);
        printf("Role: %s\n", staff[i]->role);
        printf("Email: %s\n", staff[i]->email);
        printf("Phone: %s\n", staff[i]->phone);
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(staff[i]);
    }
    free(staff);
    
    wait_for_enter();
}

void search_staff_ui() {
    print_header("Search Staff");
    int staff_id = get_int_input("Enter staff ID to search: ");
    
    Staff* staff = get_staff(staff_id);
    if (!staff) {
        printf("\nStaff not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nStaff ID: %d\n", staff->staff_id);
    printf("Name: %s\n", staff->name);
    printf("Role: %s\n", staff->role);
    printf("Email: %s\n", staff->email);
    printf("Phone: %s\n", staff->phone);
    
    free(staff);
    wait_for_enter();
}

// Borrowing menu
void show_borrowing_menu() {
    print_header("Borrowing Management");
    printf("1. Add New Borrowing\n");
    printf("2. Update Borrowing\n");
    printf("3. Delete Borrowing\n");
    printf("4. List All Borrowings\n");
    printf("5. Search Borrowing\n");
    printf("6. Return Book\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_borrowing_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_borrowing_ui();
            break;
        case 2:
            update_borrowing_ui();
            break;
        case 3:
            delete_borrowing_ui();
            break;
        case 4:
            list_borrowings_ui();
            break;
        case 5:
            search_borrowing_ui();
            break;
        case 6:
            return_book_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Borrowing UI functions
void add_borrowing_ui() {
    print_header("Add New Borrowing");
    
    Borrowing borrowing;
    memset(&borrowing, 0, sizeof(Borrowing));
    
    borrowing.book_id = get_int_input("Enter book ID: ");
    borrowing.member_id = get_int_input("Enter member ID: ");
    get_date_input("Enter borrow date", borrowing.borrow_date);
    get_date_input("Enter due date", borrowing.due_date);
    borrowing.staff_id = get_int_input("Enter staff ID: ");
    
    if (add_borrowing(&borrowing)) {
        printf("\nBorrowing added successfully!\n");
    } else {
        printf("\nFailed to add borrowing.\n");
    }
    
    wait_for_enter();
}

void update_borrowing_ui() {
    print_header("Update Borrowing");
    int borrowing_id = get_int_input("Enter borrowing ID to update: ");
    
    Borrowing* borrowing = get_borrowing(borrowing_id);
    if (!borrowing) {
        printf("\nBorrowing not found!\n");
        wait_for_enter();
        return;
    }
    
    borrowing->book_id = get_int_input("Enter new book ID (or 0 to keep current): ");
    borrowing->member_id = get_int_input("Enter new member ID (or 0 to keep current): ");
    get_date_input("Enter new borrow date (or press Enter to keep current)", borrowing->borrow_date);
    get_date_input("Enter new due date (or press Enter to keep current)", borrowing->due_date);
    get_date_input("Enter new return date (or press Enter to keep current)", borrowing->return_date);
    borrowing->staff_id = get_int_input("Enter new staff ID (or 0 to keep current): ");
    
    if (update_borrowing(borrowing)) {
        printf("\nBorrowing updated successfully!\n");
    } else {
        printf("\nFailed to update borrowing.\n");
    }
    
    free(borrowing);
    wait_for_enter();
}

void delete_borrowing_ui() {
    print_header("Delete Borrowing");
    int borrowing_id = get_int_input("Enter borrowing ID to delete: ");
    
    if (delete_borrowing(borrowing_id)) {
        printf("\nBorrowing deleted successfully!\n");
    } else {
        printf("\nFailed to delete borrowing.\n");
    }
    
    wait_for_enter();
}

void list_borrowings_ui() {
    print_header("List All Borrowings");
    
    int count;
    Borrowing** borrowings = get_all_borrowings(&count);
    
    if (!borrowings) {
        printf("\nNo borrowings found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal borrowings: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Borrowing ID: %d\n", borrowings[i]->borrowing_id);
        printf("Book ID: %d\n", borrowings[i]->book_id);
        printf("Member ID: %d\n", borrowings[i]->member_id);
        printf("Borrow Date: %s\n", borrowings[i]->borrow_date);
        printf("Due Date: %s\n", borrowings[i]->due_date);
        printf("Return Date: %s\n", borrowings[i]->return_date);
        printf("Staff ID: %d\n", borrowings[i]->staff_id);
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(borrowings[i]);
    }
    free(borrowings);
    
    wait_for_enter();
}

void search_borrowing_ui() {
    print_header("Search Borrowing");
    int borrowing_id = get_int_input("Enter borrowing ID to search: ");
    
    Borrowing* borrowing = get_borrowing(borrowing_id);
    if (!borrowing) {
        printf("\nBorrowing not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nBorrowing ID: %d\n", borrowing->borrowing_id);
    printf("Book ID: %d\n", borrowing->book_id);
    printf("Member ID: %d\n", borrowing->member_id);
    printf("Borrow Date: %s\n", borrowing->borrow_date);
    printf("Due Date: %s\n", borrowing->due_date);
    printf("Return Date: %s\n", borrowing->return_date);
    printf("Staff ID: %d\n", borrowing->staff_id);
    
    free(borrowing);
    wait_for_enter();
}

void return_book_ui() {
    print_header("Return Book");
    int borrowing_id = get_int_input("Enter borrowing ID: ");
    
    Borrowing* borrowing = get_borrowing(borrowing_id);
    if (!borrowing) {
        printf("\nBorrowing not found!\n");
        wait_for_enter();
        return;
    }
    
    if (strlen(borrowing->return_date) > 0) {
        printf("\nBook already returned!\n");
        free(borrowing);
        wait_for_enter();
        return;
    }
    
    get_date_input("Enter return date", borrowing->return_date);
    
    if (update_borrowing(borrowing)) {
        printf("\nBook returned successfully!\n");
    } else {
        printf("\nFailed to return book.\n");
    }
    
    free(borrowing);
    wait_for_enter();
}

// Fine menu
void show_fine_menu() {
    print_header("Fine Management");
    printf("1. Add New Fine\n");
    printf("2. Update Fine\n");
    printf("3. Delete Fine\n");
    printf("4. List All Fines\n");
    printf("5. Search Fine\n");
    printf("6. Pay Fine\n");
    printf("0. Back to Main Menu\n");
    printf("\nEnter your choice: ");
}

void handle_fine_menu_choice(int choice) {
    switch (choice) {
        case 1:
            add_fine_ui();
            break;
        case 2:
            update_fine_ui();
            break;
        case 3:
            delete_fine_ui();
            break;
        case 4:
            list_fines_ui();
            break;
        case 5:
            search_fine_ui();
            break;
        case 6:
            pay_fine_ui();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            wait_for_enter();
    }
}

// Fine UI functions
void add_fine_ui() {
    print_header("Add New Fine");
    
    Fine fine;
    memset(&fine, 0, sizeof(Fine));
    
    fine.borrowing_id = get_int_input("Enter borrowing ID: ");
    fine.amount = get_double_input("Enter fine amount: ");
    fine.paid = 0;
    
    if (add_fine(&fine)) {
        printf("\nFine added successfully!\n");
    } else {
        printf("\nFailed to add fine.\n");
    }
    
    wait_for_enter();
}

void update_fine_ui() {
    print_header("Update Fine");
    int fine_id = get_int_input("Enter fine ID to update: ");
    
    Fine* fine = get_fine(fine_id);
    if (!fine) {
        printf("\nFine not found!\n");
        wait_for_enter();
        return;
    }
    
    fine->borrowing_id = get_int_input("Enter new borrowing ID (or 0 to keep current): ");
    fine->amount = get_double_input("Enter new amount (or 0 to keep current): ");
    fine->paid = get_int_input("Enter paid status (1 for paid, 0 for unpaid): ");
    if (fine->paid) {
        get_date_input("Enter date paid", fine->date_paid);
    }
    
    if (update_fine(fine)) {
        printf("\nFine updated successfully!\n");
    } else {
        printf("\nFailed to update fine.\n");
    }
    
    free(fine);
    wait_for_enter();
}

void delete_fine_ui() {
    print_header("Delete Fine");
    int fine_id = get_int_input("Enter fine ID to delete: ");
    
    if (delete_fine(fine_id)) {
        printf("\nFine deleted successfully!\n");
    } else {
        printf("\nFailed to delete fine.\n");
    }
    
    wait_for_enter();
}

void list_fines_ui() {
    print_header("List All Fines");
    
    int count;
    Fine** fines = get_all_fines(&count);
    
    if (!fines) {
        printf("\nNo fines found.\n");
        wait_for_enter();
        return;
    }
    
    printf("\nTotal fines: %d\n\n", count);
    for (int i = 0; i < count; i++) {
        printf("Fine ID: %d\n", fines[i]->fine_id);
        printf("Borrowing ID: %d\n", fines[i]->borrowing_id);
        printf("Amount: %.2f\n", fines[i]->amount);
        printf("Paid: %s\n", fines[i]->paid ? "Yes" : "No");
        if (fines[i]->paid) {
            printf("Date Paid: %s\n", fines[i]->date_paid);
        }
        printf("\n");
    }
    
    for (int i = 0; i < count; i++) {
        free(fines[i]);
    }
    free(fines);
    
    wait_for_enter();
}

void search_fine_ui() {
    print_header("Search Fine");
    int fine_id = get_int_input("Enter fine ID to search: ");
    
    Fine* fine = get_fine(fine_id);
    if (!fine) {
        printf("\nFine not found!\n");
        wait_for_enter();
        return;
    }
    
    printf("\nFine ID: %d\n", fine->fine_id);
    printf("Borrowing ID: %d\n", fine->borrowing_id);
    printf("Amount: %.2f\n", fine->amount);
    printf("Paid: %s\n", fine->paid ? "Yes" : "No");
    if (fine->paid) {
        printf("Date Paid: %s\n", fine->date_paid);
    }
    
    free(fine);
    wait_for_enter();
}

void pay_fine_ui() {
    print_header("Pay Fine");
    int fine_id = get_int_input("Enter fine ID: ");
    
    Fine* fine = get_fine(fine_id);
    if (!fine) {
        printf("\nFine not found!\n");
        wait_for_enter();
        return;
    }
    
    if (fine->paid) {
        printf("\nFine already paid!\n");
        free(fine);
        wait_for_enter();
        return;
    }
    
    fine->paid = 1;
    get_date_input("Enter payment date", fine->date_paid);
    
    if (update_fine(fine)) {
        printf("\nFine paid successfully!\n");
    } else {
        printf("\nFailed to pay fine.\n");
    }
    
    free(fine);
    wait_for_enter();
}

int main() {
    ensure_data_directory();
    
    while (1) {
        show_main_menu();
        int choice = get_int_input("");
        handle_main_menu_choice(choice);
    }
    
    return 0;
} 