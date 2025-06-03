#define UNICODE
#define _UNICODE
#define WINVER 0x0601
#define _WIN32_IE 0x0600

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_ops.h"
#include "models.h"

// Link with required libraries
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "uuid.lib")

// Window handles
HWND hMainWindow;
HWND hTabControl;
HWND hBookList;
HWND hAuthorList;
HWND hPublisherList;
HWND hMemberList;
HWND hStaffList;
HWND hBorrowingList;
HWND hFineList;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeGUI(HWND hwnd);
void CreateTabControl(HWND hwnd);
void CreateBookControls(HWND hwnd);
void CreateAuthorControls(HWND hwnd);
void CreatePublisherControls(HWND hwnd);
void CreateMemberControls(HWND hwnd);
void CreateStaffControls(HWND hwnd);
void CreateBorrowingControls(HWND hwnd);
void CreateFineControls(HWND hwnd);
void RefreshBookList();
void RefreshAuthorList();
void RefreshPublisherList();
void RefreshMemberList();
void RefreshStaffList();
void RefreshBorrowingList();
void RefreshFineList();

// Window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TAB_HEIGHT 400
#define LIST_HEIGHT 300
#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH 100
#define MARGIN 10

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Ensure data directory exists
    ensure_data_directory();

    // Register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"LibraryManagementSystem";

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create main window
    hMainWindow = CreateWindowExW(
        0,
        L"LibraryManagementSystem",
        L"Library Management System",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hMainWindow) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Initialize GUI components
    InitializeGUI(hMainWindow);

    // Show window
    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateTabControl(hwnd);
            return 0;

        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            if (pnmh->code == TCN_SELCHANGE) {
                int iPage = TabCtrl_GetCurSel(hTabControl);
                switch (iPage) {
                    case 0: // Books
                        ShowWindow(hBookList, SW_SHOW);
                        ShowWindow(hAuthorList, SW_HIDE);
                        ShowWindow(hPublisherList, SW_HIDE);
                        ShowWindow(hMemberList, SW_HIDE);
                        ShowWindow(hStaffList, SW_HIDE);
                        ShowWindow(hBorrowingList, SW_HIDE);
                        ShowWindow(hFineList, SW_HIDE);
                        RefreshBookList();
                        break;
                    case 1: // Authors
                        ShowWindow(hBookList, SW_HIDE);
                        ShowWindow(hAuthorList, SW_SHOW);
                        ShowWindow(hPublisherList, SW_HIDE);
                        ShowWindow(hMemberList, SW_HIDE);
                        ShowWindow(hStaffList, SW_HIDE);
                        ShowWindow(hBorrowingList, SW_HIDE);
                        ShowWindow(hFineList, SW_HIDE);
                        RefreshAuthorList();
                        break;
                }
            }
            return 0;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case 1: // Add Book
                    MessageBoxW(hwnd, L"Add Book functionality will be implemented", L"Information", MB_OK);
                    break;
                case 2: // Edit Book
                    MessageBoxW(hwnd, L"Edit Book functionality will be implemented", L"Information", MB_OK);
                    break;
                case 3: // Delete Book
                    MessageBoxW(hwnd, L"Delete Book functionality will be implemented", L"Information", MB_OK);
                    break;
                case 4: // Add Author
                    MessageBoxW(hwnd, L"Add Author functionality will be implemented", L"Information", MB_OK);
                    break;
                case 5: // Edit Author
                    MessageBoxW(hwnd, L"Edit Author functionality will be implemented", L"Information", MB_OK);
                    break;
                case 6: // Delete Author
                    MessageBoxW(hwnd, L"Delete Author functionality will be implemented", L"Information", MB_OK);
                    break;
            }
            return 0;
        }

        case WM_SIZE: {
            // Resize the tab control and list views when the window is resized
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            
            // Resize tab control
            SetWindowPos(hTabControl, NULL,
                0, 0,
                rcClient.right, TAB_HEIGHT,
                SWP_NOZORDER);

            // Resize list views
            int listHeight = rcClient.bottom - TAB_HEIGHT - BUTTON_HEIGHT - 2 * MARGIN;
            SetWindowPos(hBookList, NULL,
                MARGIN, TAB_HEIGHT + MARGIN,
                rcClient.right - 2 * MARGIN, listHeight,
                SWP_NOZORDER);
            SetWindowPos(hAuthorList, NULL,
                MARGIN, TAB_HEIGHT + MARGIN,
                rcClient.right - 2 * MARGIN, listHeight,
                SWP_NOZORDER);

            // Reposition buttons
            int buttonY = rcClient.bottom - BUTTON_HEIGHT - MARGIN;
            HWND hButton = GetWindow(hwnd, GW_CHILD);
            wchar_t buffer[256];
            while (hButton) {
                if (GetClassNameW(hButton, buffer, 256) && wcscmp(buffer, L"Button") == 0) {
                    RECT rcButton;
                    GetWindowRect(hButton, &rcButton);
                    int buttonWidth = rcButton.right - rcButton.left;
                    int buttonX = MARGIN;
                    SetWindowPos(hButton, NULL,
                        buttonX, buttonY,
                        buttonWidth, BUTTON_HEIGHT,
                        SWP_NOZORDER);
                    buttonX += buttonWidth + MARGIN;
                }
                hButton = GetWindow(hButton, GW_HWNDNEXT);
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void InitializeGUI(HWND hwnd) {
    // Create tab control
    CreateTabControl(hwnd);

    // Create list views for each section
    CreateBookControls(hwnd);
    CreateAuthorControls(hwnd);
    CreatePublisherControls(hwnd);
    CreateMemberControls(hwnd);
    CreateStaffControls(hwnd);
    CreateBorrowingControls(hwnd);
    CreateFineControls(hwnd);

    // Initially show only the book list
    ShowWindow(hBookList, SW_SHOW);
    ShowWindow(hAuthorList, SW_HIDE);
    ShowWindow(hPublisherList, SW_HIDE);
    ShowWindow(hMemberList, SW_HIDE);
    ShowWindow(hStaffList, SW_HIDE);
    ShowWindow(hBorrowingList, SW_HIDE);
    ShowWindow(hFineList, SW_HIDE);

    // Initial refresh of lists
    RefreshBookList();
}

void CreateTabControl(HWND hwnd) {
    hTabControl = CreateWindowEx(
        0,
        WC_TABCONTROL,
        L"",
        WS_CHILD | WS_VISIBLE,
        MARGIN, MARGIN,
        WINDOW_WIDTH - 2 * MARGIN,
        TAB_HEIGHT,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // Add tabs
    TCITEM tie;
    tie.mask = TCIF_TEXT;

    tie.pszText = L"Books";
    TabCtrl_InsertItem(hTabControl, 0, &tie);

    tie.pszText = L"Authors";
    TabCtrl_InsertItem(hTabControl, 1, &tie);

    tie.pszText = L"Publishers";
    TabCtrl_InsertItem(hTabControl, 2, &tie);

    tie.pszText = L"Members";
    TabCtrl_InsertItem(hTabControl, 3, &tie);

    tie.pszText = L"Staff";
    TabCtrl_InsertItem(hTabControl, 4, &tie);

    tie.pszText = L"Borrowings";
    TabCtrl_InsertItem(hTabControl, 5, &tie);

    tie.pszText = L"Fines";
    TabCtrl_InsertItem(hTabControl, 6, &tie);
}

void CreateBookControls(HWND hwnd) {
    // Create list view for books
    hBookList = CreateWindowEx(
        0,
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        MARGIN, MARGIN + TAB_HEIGHT,
        WINDOW_WIDTH - 2 * MARGIN,
        LIST_HEIGHT,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // Add columns
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;

    lvc.pszText = L"ID";
    lvc.cx = 50;
    ListView_InsertColumn(hBookList, 0, &lvc);

    lvc.pszText = L"Title";
    lvc.cx = 200;
    ListView_InsertColumn(hBookList, 1, &lvc);

    lvc.pszText = L"Author";
    lvc.cx = 100;
    ListView_InsertColumn(hBookList, 2, &lvc);

    lvc.pszText = L"Publisher";
    lvc.cx = 100;
    ListView_InsertColumn(hBookList, 3, &lvc);

    lvc.pszText = L"ISBN";
    lvc.cx = 100;
    ListView_InsertColumn(hBookList, 4, &lvc);

    lvc.pszText = L"Genre";
    lvc.cx = 100;
    ListView_InsertColumn(hBookList, 5, &lvc);

    lvc.pszText = L"Year";
    lvc.cx = 60;
    ListView_InsertColumn(hBookList, 6, &lvc);

    lvc.pszText = L"Copies";
    lvc.cx = 60;
    ListView_InsertColumn(hBookList, 7, &lvc);

    // Create buttons
    CreateWindow(
        L"BUTTON",
        L"Add Book",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN, MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)1,
        GetModuleHandle(NULL),
        NULL
    );

    CreateWindow(
        L"BUTTON",
        L"Edit Book",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN + BUTTON_WIDTH + MARGIN, MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)2,
        GetModuleHandle(NULL),
        NULL
    );

    CreateWindow(
        L"BUTTON",
        L"Delete Book",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN + 2 * (BUTTON_WIDTH + MARGIN), MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)3,
        GetModuleHandle(NULL),
        NULL
    );
}

void CreateAuthorControls(HWND hwnd) {
    // Create list view for authors
    hAuthorList = CreateWindowExW(
        0,
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        MARGIN, MARGIN + TAB_HEIGHT,
        WINDOW_WIDTH - 2 * MARGIN,
        LIST_HEIGHT,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // Add columns
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;

    lvc.pszText = L"ID";
    lvc.cx = 50;
    ListView_InsertColumn(hAuthorList, 0, &lvc);

    lvc.pszText = L"Name";
    lvc.cx = 200;
    ListView_InsertColumn(hAuthorList, 1, &lvc);

    lvc.pszText = L"Biography";
    lvc.cx = 300;
    ListView_InsertColumn(hAuthorList, 2, &lvc);

    // Create buttons
    CreateWindowW(
        L"BUTTON",
        L"Add Author",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN, MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)4,
        GetModuleHandle(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Edit Author",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN + BUTTON_WIDTH + MARGIN, MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)5,
        GetModuleHandle(NULL),
        NULL
    );

    CreateWindowW(
        L"BUTTON",
        L"Delete Author",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        MARGIN + 2 * (BUTTON_WIDTH + MARGIN), MARGIN + TAB_HEIGHT + LIST_HEIGHT + MARGIN,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hwnd,
        (HMENU)6,
        GetModuleHandle(NULL),
        NULL
    );
}

void RefreshAuthorList() {
    // Clear the list
    ListView_DeleteAllItems(hAuthorList);

    // Get all authors
    int count;
    Author** authors = get_all_authors(&count);

    if (authors) {
        // Add authors to the list
        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        wchar_t buffer[256];

        for (int i = 0; i < count; i++) {
            // ID
            lvi.iItem = i;
            lvi.iSubItem = 0;
            swprintf(buffer, 256, L"%ld", (long)authors[i]->author_id);
            lvi.pszText = buffer;
            ListView_InsertItem(hAuthorList, &lvi);

            // Name
            MultiByteToWideChar(CP_UTF8, 0, authors[i]->name, -1, buffer, 256);
            ListView_SetItemText(hAuthorList, i, 1, buffer);

            // Biography
            MultiByteToWideChar(CP_UTF8, 0, authors[i]->bio, -1, buffer, 256);
            ListView_SetItemText(hAuthorList, i, 2, buffer);
        }

        // Free memory
        for (int i = 0; i < count; i++) {
            free(authors[i]);
        }
        free(authors);
    }
}

// Placeholder implementations for other sections
void CreatePublisherControls(HWND hwnd) {
    // Similar to CreateAuthorControls but for publishers
}

void CreateMemberControls(HWND hwnd) {
    // Similar to CreateAuthorControls but for members
}

void CreateStaffControls(HWND hwnd) {
    // Similar to CreateAuthorControls but for staff
}

void CreateBorrowingControls(HWND hwnd) {
    // Similar to CreateAuthorControls but for borrowings
}

void CreateFineControls(HWND hwnd) {
    // Similar to CreateAuthorControls but for fines
}

void RefreshPublisherList() {
    // Similar to RefreshAuthorList but for publishers
}

void RefreshMemberList() {
    // Similar to RefreshAuthorList but for members
}

void RefreshStaffList() {
    // Similar to RefreshAuthorList but for staff
}

void RefreshBorrowingList() {
    // Similar to RefreshAuthorList but for borrowings
}

void RefreshFineList() {
    // Similar to RefreshAuthorList but for fines
}

void RefreshBookList() {
    // Clear the list
    ListView_DeleteAllItems(hBookList);

    // Get all books
    int count;
    Book** books = get_all_books(&count);

    if (books) {
        // Add books to the list
        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        wchar_t buffer[256];

        for (int i = 0; i < count; i++) {
            // ID
            lvi.iItem = i;
            lvi.iSubItem = 0;
            swprintf(buffer, 256, L"%ld", (long)books[i]->book_id);
            lvi.pszText = buffer;
            ListView_InsertItem(hBookList, &lvi);

            // Title
            MultiByteToWideChar(CP_UTF8, 0, books[i]->title, -1, buffer, 256);
            ListView_SetItemText(hBookList, i, 1, buffer);

            // Author ID
            swprintf(buffer, 256, L"%ld", (long)books[i]->author_id);
            ListView_SetItemText(hBookList, i, 2, buffer);

            // Publisher ID
            swprintf(buffer, 256, L"%ld", (long)books[i]->publisher_id);
            ListView_SetItemText(hBookList, i, 3, buffer);

            // ISBN
            MultiByteToWideChar(CP_UTF8, 0, books[i]->isbn, -1, buffer, 256);
            ListView_SetItemText(hBookList, i, 4, buffer);

            // Genre
            MultiByteToWideChar(CP_UTF8, 0, books[i]->genre, -1, buffer, 256);
            ListView_SetItemText(hBookList, i, 5, buffer);

            // Year
            swprintf(buffer, 256, L"%ld", (long)books[i]->year_published);
            ListView_SetItemText(hBookList, i, 6, buffer);

            // Copies
            swprintf(buffer, 256, L"%ld", (long)books[i]->copies_available);
            ListView_SetItemText(hBookList, i, 7, buffer);
        }

        // Free memory
        for (int i = 0; i < count; i++) {
            free(books[i]);
        }
        free(books);
    }
} 