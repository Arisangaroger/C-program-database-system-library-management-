# Library Management System

A comprehensive library management system implemented in C, featuring both console and GUI interfaces. The system provides full CRUD operations for managing books, authors, publishers, members, staff, borrowings, and fines.

## Features

- File-based persistent storage using binary files
- Full CRUD operations for all entities
- Console-based user interface
- Data validation and error handling
- Support for foreign key relationships
- Date-based operations for borrowings and fines

## Data Model

The system manages the following entities:

1. Books
   - Book details (title, ISBN, genre, etc.)
   - Author and publisher relationships
   - Inventory management (copies available)

2. Authors
   - Author information (name, bio)

3. Publishers
   - Publisher details (name, address, contact info)

4. Members
   - Member information (name, contact details)
   - Membership status tracking

5. Staff
   - Staff details (name, role, contact info)

6. Borrowings
   - Book borrowing records
   - Due date tracking
   - Return date recording

7. Fines
   - Fine records for late returns
   - Payment status tracking

## Building the Project

### Prerequisites

- GCC compiler
- Make

### Build Instructions

1. Clone the repository
2. Navigate to the project directory
3. Run `make` to build the project
4. Run `./library_system` to start the application

### Clean Build

To clean the build files:
```bash
make clean
```

## Usage

1. Start the application
2. Use the menu-driven interface to:
   - Add, update, delete, and search records
   - Manage book borrowings
   - Track fines and payments
   - Generate reports

## File Structure

- `models.h` - Data structures and type definitions
- `file_ops.h/c` - File operations and CRUD functions
- `console_ui.h/c` - Console interface implementation
- `Makefile` - Build configuration

## Data Storage

All data is stored in binary files in the `data` directory:
- `authors.dat`
- `publishers.dat`
- `books.dat`
- `members.dat`
- `staff.dat`
- `borrowings.dat`
- `fines.dat`

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details. 