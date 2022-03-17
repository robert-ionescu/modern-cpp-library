#pragma once
#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include <stdio.h>
#include <sstream>
#include "Logs.h"



//#include "../User/User.h"
//#include "Author.h"

#define logl(x) std::cout << x << std::endl



struct Data {
	uint32_t rows = 0;
	std::vector<std::string> vector;
};
class Database
{
private:
	sqlite3* database;
	char* dbError;
	uint16_t status;
	Logs logs;

public:
	Database();
	sqlite3* GetDatabase() const;
	void SetDatabase(sqlite3* database);
	bool OpenDatabase(const char* path);
	bool ExecuteQuery(const std::string&);
	void ExecuteQuery1();
	bool GetQueryResult(const std::string&, Data&);
	void DisplayBookTable();
	bool AddUserTable();
	std::string AddUser(std::string firstName, std::string lastName, std::string email, std::string username, std::string password);
	std::string VerifyUserLogIn(const std::string& username, const std::string& password, Database& database);
	void DisplayUserTable();
	bool AddBorrowsTable();
	std::string AddBorrows(int user_id, int book_id, std::string taken_date, std::string brought_date);
	bool AddBooks( std::string book_title, std::string book_author, std::string isbn13, int book_year);
	std::string ShowBorrows(int userID);
	std::string DeleteUser(int userId);
	bool DeleteAllBorrowsOfUser(int userId);
	std::string DeleteBorrow(int userId, int bookId);
	bool HasBorrows(int userId); //this function returns false if the current user has no borrows...deleteBorrow() can be aplied
	std::string readBookById(int bookId);
	bool Has5BorrowsAlready(int userId);//this function returns true if the user has 5 borrows
	bool NoMoreExamplesOfBook(int bookId);//this function returns true if the same book was already borrowed 3 times 
	std::string getPreviewBook(int bookId);
	std::string ExtendBorrow(int userId, int bookId);
	void ConnectTables();
	void CloseDatabase();
	void CreateVirtualTables();
	std::string Spellfix(std::string searchTerm);
	std::string Search(std::string term); //sa intoarca id-ul?
	std::string GetBookByTitle(std::string title);
	std::string GetBookByAuthor(std::string author);


	~Database();
};
static int callback(void* data, int, char**, char**);