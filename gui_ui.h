#ifndef GUI_UI_H
#define GUI_UI_H

#include <windows.h>

// Window handles
extern HWND hMainWindow;
extern HWND hTabControl;
extern HWND hBookList;
extern HWND hAuthorList;
extern HWND hPublisherList;
extern HWND hMemberList;
extern HWND hStaffList;
extern HWND hBorrowingList;
extern HWND hFineList;

// Function declarations
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

#endif // GUI_UI_H 