#include <gtk/gtk.h>
#include "file_ops.h"
#include <ctype.h>  // Add this for isdigit()
#include "models.h"

// Global variables
GtkWidget *window;
GtkWidget *notebook;
GtkWidget *book_list;
GtkWidget *author_list;
GtkWidget *publisher_list;
GtkListStore *book_store;
GtkWidget *book_search_entry;

// Column indices for book list
#define BOOK_ID_COLUMN 0
#define BOOK_TITLE_COLUMN 1
#define BOOK_AUTHOR_COLUMN 2
#define BOOK_PUBLISHER_COLUMN 3
#define BOOK_ISBN_COLUMN 4
#define BOOK_GENRE_COLUMN 5
#define BOOK_YEAR_COLUMN 6
#define BOOK_COPIES_COLUMN 7
#define BOOK_LOCATION_COLUMN 8

// Function declarations
void create_main_window(void);
void create_book_section(void);
void create_author_section(void);
void create_publisher_section(void);
void refresh_book_list(void);
void refresh_author_list(void);
void refresh_publisher_list(void);
static void switch_to_books_tab(void);  // Add this declaration

// Callback function declarations
static void on_add_book_clicked(GtkWidget *widget, gpointer data);
static void on_edit_book_clicked(GtkWidget *widget, gpointer data);
static void on_delete_book_clicked(GtkWidget *widget, gpointer data);
static void on_add_author_clicked(GtkWidget *widget, gpointer data);
static void on_edit_author_clicked(GtkWidget *widget, gpointer data);
static void on_delete_author_clicked(GtkWidget *widget, gpointer data);
static void on_add_publisher_clicked(GtkWidget *widget, gpointer data);
static void on_edit_publisher_clicked(GtkWidget *widget, gpointer data);
static void on_delete_publisher_clicked(GtkWidget *widget, gpointer data);

// Publisher dialog function declarations
static void show_add_publisher_dialog(GtkWidget *parent);
static void show_edit_publisher_dialog(GtkWidget *parent, int publisher_id);
static void show_delete_publisher_dialog(GtkWidget *parent, int publisher_id);
static int get_selected_publisher_id(void);

// Search callback
static void on_book_search_changed(GtkWidget *widget, gpointer data) {
    const char *search_text = gtk_entry_get_text(GTK_ENTRY(book_search_entry));
    refresh_book_list();  // This will now filter based on search text
}

// Function to validate numeric input
static int validate_numeric_input(const char* input, int min_value, int max_value) {
    char* endptr;
    long value = strtol(input, &endptr, 10);
    
    // Check if the entire string was converted
    if (*endptr != '\0') {
        return 0;
    }
    
    // Check if the value is within range
    if (value < min_value || value > max_value) {
        return 0;
    }
    
    return 1;
}

// Add this function to refresh the combo boxes
static void refresh_book_dialog_combos(GtkWidget *author_combo, GtkWidget *publisher_combo) {
    // Clear existing items
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(author_combo));
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(publisher_combo));

    // Refresh authors
    int author_count;
    Author** authors = get_all_authors(&author_count);
    if (authors) {
        for (int i = 0; i < author_count; i++) {
            char author_text[256];
            snprintf(author_text, sizeof(author_text), "%d - %s", authors[i]->author_id, authors[i]->name);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(author_combo), author_text);
            free(authors[i]);
        }
        free(authors);
    }

    // Refresh publishers
    int publisher_count;
    Publisher** publishers = get_all_publishers(&publisher_count);
    if (publishers) {
        for (int i = 0; i < publisher_count; i++) {
            char publisher_text[256];
            snprintf(publisher_text, sizeof(publisher_text), "%d - %s", publishers[i]->publisher_id, publishers[i]->name);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(publisher_combo), publisher_text);
            free(publishers[i]);
        }
        free(publishers);
    }
}

// Function to show the Add Book dialog
static void show_add_book_dialog(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Book", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Add", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *title_label = gtk_label_new("Title:");
    GtkWidget *title_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), title_entry, 1, 0, 1, 1);

    // Author input (changed from combo to entry)
    GtkWidget *author_label = gtk_label_new("Author:");
    GtkWidget *author_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), author_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), author_entry, 1, 1, 1, 1);

    // Publisher input (changed from combo to entry)
    GtkWidget *publisher_label = gtk_label_new("Publisher:");
    GtkWidget *publisher_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), publisher_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), publisher_entry, 1, 2, 1, 1);

    GtkWidget *isbn_label = gtk_label_new("ISBN:");
    GtkWidget *isbn_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), isbn_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), isbn_entry, 1, 3, 1, 1);

    GtkWidget *genre_label = gtk_label_new("Genre:");
    GtkWidget *genre_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), genre_label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), genre_entry, 1, 4, 1, 1);

    GtkWidget *year_label = gtk_label_new("Year Published:");
    GtkWidget *year_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), year_label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), year_entry, 1, 5, 1, 1);

    GtkWidget *copies_label = gtk_label_new("Copies Available:");
    GtkWidget *copies_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), copies_label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), copies_entry, 1, 6, 1, 1);

    GtkWidget *shelf_label = gtk_label_new("Shelf Location:");
    GtkWidget *shelf_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), shelf_label, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shelf_entry, 1, 7, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response;
    do {
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_OK) {
            // Get values from entries
            const char *title = gtk_entry_get_text(GTK_ENTRY(title_entry));
            const char *author_name = gtk_entry_get_text(GTK_ENTRY(author_entry));
            const char *publisher_name = gtk_entry_get_text(GTK_ENTRY(publisher_entry));
            const char *isbn = gtk_entry_get_text(GTK_ENTRY(isbn_entry));
            const char *genre = gtk_entry_get_text(GTK_ENTRY(genre_entry));
            const char *year_str = gtk_entry_get_text(GTK_ENTRY(year_entry));
            const char *copies_str = gtk_entry_get_text(GTK_ENTRY(copies_entry));
            const char *shelf = gtk_entry_get_text(GTK_ENTRY(shelf_entry));

            // Validate inputs
            if (strlen(title) == 0 || strlen(author_name) == 0 || strlen(publisher_name) == 0 ||
                strlen(isbn) == 0 || strlen(genre) == 0 || strlen(year_str) == 0 ||
                strlen(copies_str) == 0 || strlen(shelf) == 0) {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "All fields are required!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                continue;  // Continue the loop instead of returning
            }

            // Validate numeric inputs
            if (!validate_numeric_input(year_str, 1000, 9999)) {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Year must be between 1000 and 9999!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                continue;  // Continue the loop instead of returning
            }

            if (!validate_numeric_input(copies_str, 0, 9999)) {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Number of copies must be between 0 and 9999!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                continue;  // Continue the loop instead of returning
            }

            // Create new author if it doesn't exist
            Author *author = (Author *)malloc(sizeof(Author));
            if (author) {
                strncpy(author->name, author_name, MAX_STRING_LENGTH - 1);
                strncpy(author->bio, "", MAX_BIO_LENGTH - 1);  // Empty bio for new authors
                if (!add_author(author)) {
                    free(author);
                    GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                   GTK_MESSAGE_ERROR,
                                                                   GTK_BUTTONS_OK,
                                                                   "Failed to add author!");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                    continue;  // Continue the loop instead of returning
                }
                free(author);
            }

            // Create new publisher if it doesn't exist
            Publisher *publisher = (Publisher *)malloc(sizeof(Publisher));
            if (publisher) {
                strncpy(publisher->name, publisher_name, MAX_STRING_LENGTH - 1);
                strncpy(publisher->address, "", MAX_STRING_LENGTH - 1);
                strncpy(publisher->phone, "", MAX_STRING_LENGTH - 1);
                if (!add_publisher(publisher)) {
                    free(publisher);
                    GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                   GTK_MESSAGE_ERROR,
                                                                   GTK_BUTTONS_OK,
                                                                   "Failed to add publisher!");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                    continue;  // Continue the loop instead of returning
                }
                free(publisher);
            }

            // Get the IDs of the newly created author and publisher
            int author_count;
            Author** authors = get_all_authors(&author_count);
            int author_id = -1;
            if (authors) {
                for (int i = 0; i < author_count; i++) {
                    if (strcmp(authors[i]->name, author_name) == 0) {
                        author_id = authors[i]->author_id;
                        break;
                    }
                    free(authors[i]);
                }
                free(authors);
            }

            int publisher_count;
            Publisher** publishers = get_all_publishers(&publisher_count);
            int publisher_id = -1;
            if (publishers) {
                for (int i = 0; i < publisher_count; i++) {
                    if (strcmp(publishers[i]->name, publisher_name) == 0) {
                        publisher_id = publishers[i]->publisher_id;
                        break;
                    }
                    free(publishers[i]);
                }
                free(publishers);
            }

            if (author_id == -1 || publisher_id == -1) {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Failed to get author or publisher ID!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                continue;  // Continue the loop instead of returning
            }

            // Convert numeric values
            int year_published = atoi(year_str);
            int copies_available = atoi(copies_str);

            // Create new book
            Book *new_book = (Book *)malloc(sizeof(Book));
            if (new_book) {
                strncpy(new_book->title, title, MAX_STRING_LENGTH - 1);
                new_book->author_id = author_id;
                new_book->publisher_id = publisher_id;
                strncpy(new_book->isbn, isbn, MAX_STRING_LENGTH - 1);
                strncpy(new_book->genre, genre, MAX_STRING_LENGTH - 1);
                new_book->year_published = year_published;
                new_book->copies_available = copies_available;
                strncpy(new_book->shelf_location, shelf, MAX_STRING_LENGTH - 1);

                // Add the book to the data
                if (add_book(new_book)) {
                    // Refresh the book list
                    refresh_book_list();
                    
                    // Show success message
                    GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                     GTK_MESSAGE_INFO,
                                                                     GTK_BUTTONS_OK,
                                                                     "Book added successfully!");
                    gtk_dialog_run(GTK_DIALOG(success_dialog));
                    gtk_widget_destroy(success_dialog);
                    switch_to_books_tab();  // Switch to Books tab
                    break;  // Break the loop on success
                } else {
                    // Show error message
                    GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                   GTK_MESSAGE_ERROR,
                                                                   GTK_BUTTONS_OK,
                                                                   "Failed to add book!");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                }

                free(new_book);
            }
        }
    } while (response == GTK_RESPONSE_OK);

    gtk_widget_destroy(dialog);
}

// Function to get the selected book ID from the list
static int get_selected_book_id(void) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(book_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int book_id;
        gtk_tree_model_get(model, &iter, 0, &book_id, -1);
        return book_id;
    }
    return -1;
}

// Function to show the Edit Book dialog
static void show_edit_book_dialog(GtkWidget *parent, int book_id) {
    // Get the book data
    Book *book = get_book_by_id(book_id);
    if (!book) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Book not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Book", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Save", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *title_label = gtk_label_new("Title:");
    GtkWidget *title_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(title_entry), book->title);
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), title_entry, 1, 0, 1, 1);

    // Author selection
    GtkWidget *author_label = gtk_label_new("Author:");
    GtkWidget *author_combo = gtk_combo_box_text_new();
    int author_count;
    Author** authors = get_all_authors(&author_count);
    int selected_author_index = -1;
    if (authors) {
        for (int i = 0; i < author_count; i++) {
            char author_text[256];
            snprintf(author_text, sizeof(author_text), "%d - %s", authors[i]->author_id, authors[i]->name);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(author_combo), author_text);
            if (authors[i]->author_id == book->author_id) {
                selected_author_index = i;
            }
            free(authors[i]);
        }
        free(authors);
    }
    if (selected_author_index != -1) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(author_combo), selected_author_index);
    }
    gtk_grid_attach(GTK_GRID(grid), author_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), author_combo, 1, 1, 1, 1);

    // Publisher selection
    GtkWidget *publisher_label = gtk_label_new("Publisher:");
    GtkWidget *publisher_combo = gtk_combo_box_text_new();
    int publisher_count;
    Publisher** publishers = get_all_publishers(&publisher_count);
    int selected_publisher_index = -1;
    if (publishers) {
        for (int i = 0; i < publisher_count; i++) {
            char publisher_text[256];
            snprintf(publisher_text, sizeof(publisher_text), "%d - %s", publishers[i]->publisher_id, publishers[i]->name);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(publisher_combo), publisher_text);
            if (publishers[i]->publisher_id == book->publisher_id) {
                selected_publisher_index = i;
            }
            free(publishers[i]);
        }
        free(publishers);
    }
    if (selected_publisher_index != -1) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(publisher_combo), selected_publisher_index);
    }
    gtk_grid_attach(GTK_GRID(grid), publisher_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), publisher_combo, 1, 2, 1, 1);

    GtkWidget *isbn_label = gtk_label_new("ISBN:");
    GtkWidget *isbn_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(isbn_entry), book->isbn);
    gtk_grid_attach(GTK_GRID(grid), isbn_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), isbn_entry, 1, 3, 1, 1);

    GtkWidget *genre_label = gtk_label_new("Genre:");
    GtkWidget *genre_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(genre_entry), book->genre);
    gtk_grid_attach(GTK_GRID(grid), genre_label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), genre_entry, 1, 4, 1, 1);

    GtkWidget *year_label = gtk_label_new("Year Published:");
    GtkWidget *year_entry = gtk_entry_new();
    char year_str[32];
    sprintf(year_str, "%d", book->year_published);
    gtk_entry_set_text(GTK_ENTRY(year_entry), year_str);
    gtk_grid_attach(GTK_GRID(grid), year_label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), year_entry, 1, 5, 1, 1);

    GtkWidget *copies_label = gtk_label_new("Copies Available:");
    GtkWidget *copies_entry = gtk_entry_new();
    char copies_str[32];
    sprintf(copies_str, "%d", book->copies_available);
    gtk_entry_set_text(GTK_ENTRY(copies_entry), copies_str);
    gtk_grid_attach(GTK_GRID(grid), copies_label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), copies_entry, 1, 6, 1, 1);

    GtkWidget *shelf_label = gtk_label_new("Shelf Location:");
    GtkWidget *shelf_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(shelf_entry), book->shelf_location);
    gtk_grid_attach(GTK_GRID(grid), shelf_label, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), shelf_entry, 1, 7, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        // Get values from entries
        const char *title = gtk_entry_get_text(GTK_ENTRY(title_entry));
        const char *isbn = gtk_entry_get_text(GTK_ENTRY(isbn_entry));
        const char *genre = gtk_entry_get_text(GTK_ENTRY(genre_entry));
        const char *year_str = gtk_entry_get_text(GTK_ENTRY(year_entry));
        const char *copies_str = gtk_entry_get_text(GTK_ENTRY(copies_entry));
        const char *shelf = gtk_entry_get_text(GTK_ENTRY(shelf_entry));

        // Get selected author and publisher IDs
        int author_index = gtk_combo_box_get_active(GTK_COMBO_BOX(author_combo));
        int publisher_index = gtk_combo_box_get_active(GTK_COMBO_BOX(publisher_combo));

        // Validate inputs
        if (strlen(title) == 0 || author_index == -1 || publisher_index == -1 ||
            strlen(isbn) == 0 || strlen(genre) == 0 || strlen(year_str) == 0 ||
            strlen(copies_str) == 0 || strlen(shelf) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "All fields are required!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(book);
            return;
        }

        // Validate numeric inputs
        if (!validate_numeric_input(year_str, 1000, 9999)) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Year must be between 1000 and 9999!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(book);
            return;
        }

        if (!validate_numeric_input(copies_str, 0, 9999)) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Number of copies must be between 0 and 9999!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(book);
            return;
        }

        // Get author and publisher IDs from the selected items
        int author_id = -1;
        int publisher_id = -1;
        
        // Get author ID
        authors = get_all_authors(&author_count);
        if (authors && author_index >= 0 && author_index < author_count) {
            author_id = authors[author_index]->author_id;
            for (int i = 0; i < author_count; i++) {
                free(authors[i]);
            }
            free(authors);
        }

        // Get publisher ID
        publishers = get_all_publishers(&publisher_count);
        if (publishers && publisher_index >= 0 && publisher_index < publisher_count) {
            publisher_id = publishers[publisher_index]->publisher_id;
            for (int i = 0; i < publisher_count; i++) {
                free(publishers[i]);
            }
            free(publishers);
        }

        if (author_id == -1 || publisher_id == -1) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Invalid author or publisher selection!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(book);
            return;
        }

        // Convert numeric values
        int year_published = atoi(year_str);
        int copies_available = atoi(copies_str);

        // Update book data
        strncpy(book->title, title, MAX_STRING_LENGTH - 1);
        book->author_id = author_id;
        book->publisher_id = publisher_id;
        strncpy(book->isbn, isbn, MAX_STRING_LENGTH - 1);
        strncpy(book->genre, genre, MAX_STRING_LENGTH - 1);
        book->year_published = year_published;
        book->copies_available = copies_available;
        strncpy(book->shelf_location, shelf, MAX_STRING_LENGTH - 1);

        // Update the book in the data
        if (update_book(book)) {
            // Refresh the book list
            refresh_book_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Book updated successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
            switch_to_books_tab();  // Switch to Books tab
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to update book!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(book);
}

// Function to show the Delete Book confirmation dialog
static void show_delete_book_dialog(GtkWidget *parent, int book_id) {
    // Get the book data to show in confirmation
    Book *book = get_book_by_id(book_id);
    if (!book) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Book not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    // Create confirmation dialog
    char message[256];
    snprintf(message, sizeof(message), "Are you sure you want to delete the book:\n\nTitle: %s\nAuthor ID: %d\nISBN: %s",
             book->title, book->author_id, book->isbn);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete");

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_YES) {
        // Delete the book
        if (delete_book(book_id)) {
            // Refresh the book list
            refresh_book_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Book deleted successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to delete book!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(book);
}

// Function to show the Add Author dialog
static void show_add_author_dialog(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Author", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Add", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);

    GtkWidget *bio_label = gtk_label_new("Biography:");
    GtkWidget *bio_scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *bio_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(bio_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(bio_scrolled), bio_text);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bio_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(bio_scrolled, 300, 200);  // Made taller for more text
    gtk_grid_attach(GTK_GRID(grid), bio_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bio_scrolled, 1, 1, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(bio_text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        const char *bio = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        // Validate inputs
        if (strlen(name) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Author name is required!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            return;
        }

        // Create new author
        Author *new_author = (Author *)malloc(sizeof(Author));
        if (new_author) {
            strncpy(new_author->name, name, MAX_STRING_LENGTH - 1);
            strncpy(new_author->bio, bio, MAX_BIO_LENGTH - 1);  // Use MAX_BIO_LENGTH instead of MAX_STRING_LENGTH

            // Add the author to the data
            if (add_author(new_author)) {
                // Refresh the author list
                refresh_author_list();
                
                // Show success message
                GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                 GTK_MESSAGE_INFO,
                                                                 GTK_BUTTONS_OK,
                                                                 "Author added successfully!");
                gtk_dialog_run(GTK_DIALOG(success_dialog));
                gtk_widget_destroy(success_dialog);
            } else {
                // Show error message
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Failed to add author!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }

            free(new_author);
        }
    }

    gtk_widget_destroy(dialog);
}

// Function to show the Edit Author dialog
static void show_edit_author_dialog(GtkWidget *parent, int author_id) {
    // Get the author data
    Author *author = get_author_by_id(author_id);
    if (!author) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Author not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Author", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Save", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name_entry), author->name);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);

    GtkWidget *bio_label = gtk_label_new("Biography:");
    GtkWidget *bio_scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *bio_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(bio_text), GTK_WRAP_WORD);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(bio_text));
    gtk_text_buffer_set_text(buffer, author->bio, -1);
    gtk_container_add(GTK_CONTAINER(bio_scrolled), bio_text);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(bio_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(bio_scrolled, 300, 200);  // Made taller for more text
    gtk_grid_attach(GTK_GRID(grid), bio_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bio_scrolled, 1, 1, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(bio_text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        const char *bio = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        // Validate inputs
        if (strlen(name) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Author name is required!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(author);
            return;
        }

        // Update author data
        strncpy(author->name, name, MAX_STRING_LENGTH - 1);
        strncpy(author->bio, bio, MAX_BIO_LENGTH - 1);  // Use MAX_BIO_LENGTH instead of MAX_STRING_LENGTH

        // Update the author in the data
        if (update_author(author)) {
            // Refresh the author list
            refresh_author_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Author updated successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to update author!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(author);
}

// Function to show the Delete Author confirmation dialog
static void show_delete_author_dialog(GtkWidget *parent, int author_id) {
    // Get the author data to show in confirmation
    Author *author = get_author_by_id(author_id);
    if (!author) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Author not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    // Create confirmation dialog
    char message[256];
    snprintf(message, sizeof(message), "Are you sure you want to delete the author:\n\nName: %s", author->name);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete");

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_YES) {
        // Delete the author
        if (delete_author(author_id)) {
            // Refresh the author list
            refresh_author_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Author deleted successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to delete author!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(author);
}

// Function to get the selected author ID from the list
static int get_selected_author_id(void) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(author_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int author_id;
        gtk_tree_model_get(model, &iter, 0, &author_id, -1);
        return author_id;
    }
    return -1;
}

// Callback implementations
static void on_add_book_clicked(GtkWidget *widget, gpointer data) {
    g_print("Add Book button clicked\n");
    show_add_book_dialog(window);
}

static void on_edit_book_clicked(GtkWidget *widget, gpointer data) {
    int selected_book_id = get_selected_book_id();
    if (selected_book_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select a book to edit!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_edit_book_dialog(window, selected_book_id);
}

static void on_delete_book_clicked(GtkWidget *widget, gpointer data) {
    int selected_book_id = get_selected_book_id();
    if (selected_book_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select a book to delete!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_delete_book_dialog(window, selected_book_id);
}

static void on_add_author_clicked(GtkWidget *widget, gpointer data) {
    show_add_author_dialog(window);
    // Refresh the book list to show the new author
    refresh_book_list();
}

static void on_edit_author_clicked(GtkWidget *widget, gpointer data) {
    int selected_author_id = get_selected_author_id();
    if (selected_author_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select an author to edit!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_edit_author_dialog(window, selected_author_id);
}

static void on_delete_author_clicked(GtkWidget *widget, gpointer data) {
    int selected_author_id = get_selected_author_id();
    if (selected_author_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select an author to delete!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_delete_author_dialog(window, selected_author_id);
}

// Add this validation function at the top with other functions
static int validate_phone_number(const char* phone) {
    // Check if the string is empty
    if (strlen(phone) == 0) {
        return 1;  // Allow empty phone numbers
    }
    
    // Check if all characters are digits
    for (int i = 0; phone[i] != '\0'; i++) {
        if (!isdigit(phone[i])) {
            return 0;  // Not a valid phone number
        }
    }
    
    return 1;  // Valid phone number
}

// Function to show the Add Publisher dialog
static void show_add_publisher_dialog(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Publisher", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Add", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);

    GtkWidget *address_label = gtk_label_new("Address:");
    GtkWidget *address_scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *address_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(address_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(address_scrolled), address_text);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(address_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(address_scrolled, 300, 200);  // Made taller for more text
    gtk_grid_attach(GTK_GRID(grid), address_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), address_scrolled, 1, 1, 1, 1);

    GtkWidget *phone_label = gtk_label_new("Phone:");
    GtkWidget *phone_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), phone_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), phone_entry, 1, 2, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char *phone = gtk_entry_get_text(GTK_ENTRY(phone_entry));
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(address_text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        const char *address = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        // Validate inputs
        if (strlen(name) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Publisher name is required!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            return;
        }

        // Add phone number validation
        if (!validate_phone_number(phone)) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Phone number must contain only digits!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            return;
        }

        // Create new publisher
        Publisher *new_publisher = (Publisher *)malloc(sizeof(Publisher));
        if (new_publisher) {
            strncpy(new_publisher->name, name, MAX_STRING_LENGTH - 1);
            strncpy(new_publisher->address, address, MAX_STRING_LENGTH - 1);  // Use MAX_STRING_LENGTH instead of MAX_BIO_LENGTH
            strncpy(new_publisher->phone, phone, MAX_STRING_LENGTH - 1);

            // Add the publisher to the data
            if (add_publisher(new_publisher)) {
                // Refresh the publisher list
                refresh_publisher_list();
                
                // Show success message
                GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                 GTK_MESSAGE_INFO,
                                                                 GTK_BUTTONS_OK,
                                                                 "Publisher added successfully!");
                gtk_dialog_run(GTK_DIALOG(success_dialog));
                gtk_widget_destroy(success_dialog);
            } else {
                // Show error message
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Failed to add publisher!");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }

            free(new_publisher);
        }
    }

    gtk_widget_destroy(dialog);
}

// Function to show the Edit Publisher dialog
static void show_edit_publisher_dialog(GtkWidget *parent, int publisher_id) {
    // Get the publisher data
    Publisher *publisher = get_publisher_by_id(publisher_id);
    if (!publisher) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Publisher not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Publisher", GTK_WINDOW(parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "Save", GTK_RESPONSE_OK,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(content_area), vbox);

    // Create a grid for better layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    // Add input fields with labels
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name_entry), publisher->name);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);

    GtkWidget *address_label = gtk_label_new("Address:");
    GtkWidget *address_scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *address_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(address_text), GTK_WRAP_WORD);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(address_text));
    gtk_text_buffer_set_text(buffer, publisher->address, -1);
    gtk_container_add(GTK_CONTAINER(address_scrolled), address_text);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(address_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(address_scrolled, 300, 200);  // Made taller for more text
    gtk_grid_attach(GTK_GRID(grid), address_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), address_scrolled, 1, 1, 1, 1);

    GtkWidget *phone_label = gtk_label_new("Phone:");
    GtkWidget *phone_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(phone_entry), publisher->phone);
    gtk_grid_attach(GTK_GRID(grid), phone_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), phone_entry, 1, 2, 1, 1);

    // Add some padding
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char *phone = gtk_entry_get_text(GTK_ENTRY(phone_entry));
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(address_text));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        const char *address = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        // Validate inputs
        if (strlen(name) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Publisher name is required!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(publisher);
            return;
        }

        // Add phone number validation
        if (!validate_phone_number(phone)) {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Phone number must contain only digits!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            gtk_widget_destroy(dialog);
            free(publisher);
            return;
        }

        // Update publisher data
        strncpy(publisher->name, name, MAX_STRING_LENGTH - 1);
        strncpy(publisher->address, address, MAX_STRING_LENGTH - 1);  // Use MAX_STRING_LENGTH instead of MAX_BIO_LENGTH
        strncpy(publisher->phone, phone, MAX_STRING_LENGTH - 1);

        // Update the publisher in the data
        if (update_publisher(publisher)) {
            // Refresh the publisher list
            refresh_publisher_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Publisher updated successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to update publisher!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(publisher);
}

// Function to show the Delete Publisher confirmation dialog
static void show_delete_publisher_dialog(GtkWidget *parent, int publisher_id) {
    // Get the publisher data to show in confirmation
    Publisher *publisher = get_publisher_by_id(publisher_id);
    if (!publisher) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Publisher not found!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }

    // Create confirmation dialog
    char message[256];
    snprintf(message, sizeof(message), "Are you sure you want to delete the publisher:\n\nName: %s", publisher->name);

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete");

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_YES) {
        // Delete the publisher
        if (delete_publisher(publisher_id)) {
            // Refresh the publisher list
            refresh_publisher_list();
            
            // Show success message
            GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_INFO,
                                                             GTK_BUTTONS_OK,
                                                             "Publisher deleted successfully!");
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "Failed to delete publisher!");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    }

    gtk_widget_destroy(dialog);
    free(publisher);
}

// Function to get the selected publisher ID from the list
static int get_selected_publisher_id(void) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(publisher_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int publisher_id;
        gtk_tree_model_get(model, &iter, 0, &publisher_id, -1);
        return publisher_id;
    }
    return -1;
}

// Add these callback implementations
static void on_add_publisher_clicked(GtkWidget *widget, gpointer data) {
    show_add_publisher_dialog(window);
    // Refresh the book list to show the new publisher
    refresh_book_list();
}

static void on_edit_publisher_clicked(GtkWidget *widget, gpointer data) {
    int selected_publisher_id = get_selected_publisher_id();
    if (selected_publisher_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select a publisher to edit!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_edit_publisher_dialog(window, selected_publisher_id);
}

static void on_delete_publisher_clicked(GtkWidget *widget, gpointer data) {
    int selected_publisher_id = get_selected_publisher_id();
    if (selected_publisher_id == -1) {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_OK,
                                                       "Please select a publisher to delete!");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        return;
    }
    show_delete_publisher_dialog(window, selected_publisher_id);
}

// Add this after the function declarations at the top
static void switch_to_books_tab(void) {
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);  // 0 is the Books tab
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box for the main layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create notebook for tabs
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Create sections
    create_book_section();
    create_author_section();
    create_publisher_section();

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

void create_main_window(void) {
    // This function is now empty as the initialization is done in main()
    // We keep it for compatibility with existing code
}

void create_book_section(void) {
    // Create a vertical box for the book section
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create search bar
    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *search_label = gtk_label_new("Search:");
    book_search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(book_search_entry), "Search by title, author, publisher, ISBN, or genre");
    gtk_box_pack_start(GTK_BOX(search_box), search_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_box), book_search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), search_box, FALSE, FALSE, 0);

    // Connect search signal
    g_signal_connect(G_OBJECT(book_search_entry), "changed", G_CALLBACK(on_book_search_changed), NULL);
    
    // Create the book list
    book_list = gtk_tree_view_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), book_list);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create the list store with all columns
    book_store = gtk_list_store_new(9,
        G_TYPE_INT,     // ID
        G_TYPE_STRING,  // Title
        G_TYPE_STRING,  // Author
        G_TYPE_STRING,  // Publisher
        G_TYPE_STRING,  // ISBN
        G_TYPE_STRING,  // Genre
        G_TYPE_INT,     // Year
        G_TYPE_INT,     // Copies
        G_TYPE_STRING   // Location
    );

    // Create columns
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    // ID column (hidden)
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", BOOK_ID_COLUMN, NULL);
    gtk_tree_view_column_set_visible(column, FALSE);  // Hide the ID column
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Title column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Title", renderer, "text", BOOK_TITLE_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_TITLE_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Author column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Author", renderer, "text", BOOK_AUTHOR_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_AUTHOR_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Publisher column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Publisher", renderer, "text", BOOK_PUBLISHER_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_PUBLISHER_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // ISBN column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ISBN", renderer, "text", BOOK_ISBN_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_ISBN_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Genre column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Genre", renderer, "text", BOOK_GENRE_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_GENRE_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Year column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Year", renderer, "text", BOOK_YEAR_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_YEAR_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Copies column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Copies", renderer, "text", BOOK_COPIES_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_COPIES_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Location column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Location", renderer, "text", BOOK_LOCATION_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(column, BOOK_LOCATION_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(book_list), column);

    // Enable sorting
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(book_store), BOOK_TITLE_COLUMN, GTK_SORT_ASCENDING);

    // Set the model
    gtk_tree_view_set_model(GTK_TREE_VIEW(book_list), GTK_TREE_MODEL(book_store));

    // Create buttons
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *add_button = gtk_button_new_with_label("Add Book");
    GtkWidget *edit_button = gtk_button_new_with_label("Edit Book");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete Book");

    // Connect signals
    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(on_add_book_clicked), NULL);
    g_signal_connect(G_OBJECT(edit_button), "clicked", G_CALLBACK(on_edit_book_clicked), NULL);
    g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_book_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), edit_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Add the book section to the notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Books"));

    // Refresh the book list
    refresh_book_list();
}

void create_author_section(void) {
    // Create a vertical box for the author section
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create the author list
    author_list = gtk_tree_view_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), author_list);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create columns
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    // ID column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(author_list), column);

    // Name column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(author_list), column);

    // Bio column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Biography", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(author_list), column);

    // Create buttons
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *add_button = gtk_button_new_with_label("Add Author");
    GtkWidget *edit_button = gtk_button_new_with_label("Edit Author");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete Author");

    // Connect signals
    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(on_add_author_clicked), NULL);
    g_signal_connect(G_OBJECT(edit_button), "clicked", G_CALLBACK(on_edit_author_clicked), NULL);
    g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_author_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), edit_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Add the author section to the notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Authors"));

    // Refresh the author list
    refresh_author_list();
}

void create_publisher_section(void) {
    // Create a vertical box for the publisher section
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create the publisher list
    publisher_list = gtk_tree_view_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), publisher_list);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create columns
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    // ID column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(publisher_list), column);

    // Name column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(publisher_list), column);

    // Address column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(publisher_list), column);

    // Phone column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Phone", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(publisher_list), column);

    // Create buttons
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *add_button = gtk_button_new_with_label("Add Publisher");
    GtkWidget *edit_button = gtk_button_new_with_label("Edit Publisher");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete Publisher");

    // Connect signals
    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(on_add_publisher_clicked), NULL);
    g_signal_connect(G_OBJECT(edit_button), "clicked", G_CALLBACK(on_edit_publisher_clicked), NULL);
    g_signal_connect(G_OBJECT(delete_button), "clicked", G_CALLBACK(on_delete_publisher_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), edit_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), delete_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Add the publisher section to the notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, gtk_label_new("Publishers"));

    // Refresh the publisher list
    refresh_publisher_list();
}

void refresh_book_list(void) {
    // Clear existing items
    gtk_list_store_clear(book_store);

    // Get search text
    const char *search_text = gtk_entry_get_text(GTK_ENTRY(book_search_entry));

    // Get all books
    int count;
    Book** books = get_all_books(&count);
    if (books) {
        // Get all authors and publishers for lookup
        int author_count;
        Author** authors = get_all_authors(&author_count);
        int publisher_count;
        Publisher** publishers = get_all_publishers(&publisher_count);

        for (int i = 0; i < count; i++) {
            // Find author and publisher names
            const char* author_name = "Unknown";
            const char* publisher_name = "Unknown";

            if (authors) {
                for (int j = 0; j < author_count; j++) {
                    if (authors[j]->author_id == books[i]->author_id) {
                        author_name = authors[j]->name;
                        break;
                    }
                }
            }

            if (publishers) {
                for (int j = 0; j < publisher_count; j++) {
                    if (publishers[j]->publisher_id == books[i]->publisher_id) {
                        publisher_name = publishers[j]->name;
                        break;
                    }
                }
            }

            // If search text is provided, check if book matches
            if (strlen(search_text) > 0) {
                if (strstr(books[i]->title, search_text) == NULL &&
                    strstr(author_name, search_text) == NULL &&
                    strstr(publisher_name, search_text) == NULL &&
                    strstr(books[i]->isbn, search_text) == NULL &&
                    strstr(books[i]->genre, search_text) == NULL) {
                    free(books[i]);
                    continue;
                }
            }

            GtkTreeIter iter;
            gtk_list_store_append(book_store, &iter);
            gtk_list_store_set(book_store, &iter,
                              BOOK_ID_COLUMN, books[i]->book_id,
                              BOOK_TITLE_COLUMN, books[i]->title,
                              BOOK_AUTHOR_COLUMN, author_name,
                              BOOK_PUBLISHER_COLUMN, publisher_name,
                              BOOK_ISBN_COLUMN, books[i]->isbn,
                              BOOK_GENRE_COLUMN, books[i]->genre,
                              BOOK_YEAR_COLUMN, books[i]->year_published,
                              BOOK_COPIES_COLUMN, books[i]->copies_available,
                              BOOK_LOCATION_COLUMN, books[i]->shelf_location,
                              -1);

            free(books[i]);
        }
        free(books);

        // Free authors and publishers
        if (authors) {
            for (int i = 0; i < author_count; i++) {
                free(authors[i]);
            }
            free(authors);
        }
        if (publishers) {
            for (int i = 0; i < publisher_count; i++) {
                free(publishers[i]);
            }
            free(publishers);
        }
    }
}

void refresh_author_list(void) {
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;

    int count;
    Author** authors = get_all_authors(&count);

    if (authors) {
        for (int i = 0; i < count; i++) {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                             0, authors[i]->author_id,
                             1, authors[i]->name,
                             2, authors[i]->bio,
                             -1);
        }

        // Free memory
        for (int i = 0; i < count; i++) {
            free(authors[i]);
        }
        free(authors);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(author_list), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void refresh_publisher_list(void) {
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;

    int count;
    Publisher** publishers = get_all_publishers(&count);

    if (publishers) {
        for (int i = 0; i < count; i++) {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                             0, publishers[i]->publisher_id,
                             1, publishers[i]->name,
                             2, publishers[i]->address,
                             3, publishers[i]->phone,
                             -1);
        }

        // Free memory
        for (int i = 0; i < count; i++) {
            free(publishers[i]);
        }
        free(publishers);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(publisher_list), GTK_TREE_MODEL(store));
    g_object_unref(store);
} // End of refresh_publisher_list 