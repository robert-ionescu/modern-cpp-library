#include "Database.h"


Database::Database(): logs()
{
	OpenDatabase("Database.db");
	AddBorrowsTable();
	AddUserTable();
	Logs::EnableFileOutput("logs.txt");
	char* err = NULL;
	//sqlite3_db_config(database, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, NULL);
	sqlite3_enable_load_extension(database, 1);
	sqlite3_load_extension(database, "spellfix", 0, &err);
	if (err)
	{
		Logs::High(err);
	}
	CreateVirtualTables();
}

sqlite3* Database::GetDatabase() const
{
	return this->database;
}

void Database::SetDatabase(sqlite3* database)
{
	this->database = database;
}

bool Database::OpenDatabase(const char* path)
{
	status = sqlite3_open(path, &database);
	if (status == 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Database::ExecuteQuery(const std::string& query)
{
	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		//logl("Could not execute query");
		sqlite3_free(dbError);
		return false;
	}
	else
	{
		return true;
	}


}
void Database::ExecuteQuery1()
{
	sqlite3* db;
	sqlite3_stmt* st;
	int id = 3;
	int book = 3;
	std::string title = "KGKGG";
	std::string password = "";



	std::string sql = "INSERT INTO AUTHORS(NAME,BOOK_ID) VALUES (?,?)";

	if (sqlite3_open("Database.db", &database) == SQLITE_OK)
	{
		sqlite3_prepare(database, sql.c_str(), -1, &st, NULL);

		sqlite3_bind_text(st, 1, title.c_str(), title.length(), SQLITE_TRANSIENT);
		sqlite3_bind_int(st, 2, book);
		sqlite3_step(st);
	}
	else
	{
		std::cout << "Failed to connect\n";
	}

	sqlite3_close(database);


}

bool Database::GetQueryResult(const std::string& query, Data& data) {
	status = sqlite3_exec(database, query.c_str(), callback, &data, &dbError);

	if (status != SQLITE_OK) {
		logl("Could not execute query on GetEntry()");
		sqlite3_free(dbError);
		return false;
	}
	else return true;
}


void Database::DisplayBookTable()
{
	OpenDatabase("Database.db");
	Data data;
	GetQueryResult("SELECT id,authors,title FROM books;", data);
	for (int i = 0;i < data.vector.size();i++)
	{
		std::cout << data.vector[i] << std::endl;
	}
}

bool Database::AddUserTable()
{
	const std::string& query = "CREATE TABLE IF NOT EXISTS USERS("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"FIRSTNAME     TEXT NOT NULL, "
		"LASTNAME     TEXT NOT NULL, "
		"EMAIL      TEXT NOT NULL, "
		"USERNAME     TEXT  NOT NULL, "
		"PASSWORD   TEXT NOT NULL); ";

	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		logl("Could not execute query");
		sqlite3_free(dbError);
		return false;
	}
	else
	{
		return true;
	}

}

std::string Database::AddUser(std::string firstName, std::string lastName, std::string email, std::string username, std::string password)
{
	Data results;
	if (this->GetQueryResult("SELECT username from users where username='" + username + "'", results))
	{
		if (results.rows > 0)
		{
			return "REGISTER | FAILED";
		}
	}

	if (ExecuteQuery("INSERT INTO USERS(FIRSTNAME,LASTNAME,EMAIL,USERNAME,PASSWORD) VALUES('" + firstName + "','" + lastName + "','" + email + "','" + username + "','" + password + "')"))
	{
		return "REGISTER | OK";
	}
	return "REGISTER | FAILED";
}

std::string Database::VerifyUserLogIn(const std::string& username,const std::string& password,Database& database)
{
	Data results;
	if (this->GetQueryResult("SELECT * FROM users WHERE username = '" + username + "' LIMIT 1", results)) {
		if (results.rows > 0 && results.vector[4] == username && results.vector[5] == password)
		{
			return results.vector[0];
		}
	}
	return "LOGIN | FAILED";
}

void Database::DisplayUserTable()
{
	OpenDatabase("Database.db");
	Data data;
	GetQueryResult("SELECT "
		"ID"
		",FIRSTNAME"
		",LASTNAME"
		",EMAIL"
		",USERNAME"
		",PASSWORD "
		"FROM USERS;", data);
	for (int i = 0;i < data.vector.size();i++)
	{
		std::cout << data.vector[i] << std::endl;
	}
}

bool Database::AddBorrowsTable()
{
	const std::string& query = "CREATE TABLE IF NOT EXISTS BORROWS("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"USER_ID     INT NOT NULL, "
		"BOOK_ID     INT NOT NULL, "
		"TAKEN_DATE     DATE NOT NULL, "
		"EXTENSIONS     INT NOT NULL, "
		"BROUGHT_DATE   DATE NOT NULL); ";

	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		logl("Could not execute query");
		sqlite3_free(dbError);
		return false;
	}
	else
	{
		return true;
	}
}

std::string Database::AddBorrows(int user_id,int book_id,std::string taken_date,std::string brought_date)
{
	std::string userId_s = std::to_string(user_id);
	std::string bookId_s = std::to_string(book_id);
	if (ExecuteQuery("INSERT INTO BORROWS (USER_ID,BOOK_ID,TAKEN_DATE,EXTENSIONS,BROUGHT_DATE) VALUES('" + userId_s + "','" + bookId_s + "','" + taken_date + "',0,'" + brought_date + "');"))
	{
		std::cout << "ok";
		return "BORROW | OK";
	}
	return "BORROW | FAILED";
}

bool Database::AddBooks( std::string book_title, std::string book_author, std::string isbn13, int book_year)
{
	sqlite3_stmt* st;
	std::string sql = "INSERT INTO BOOKS(title,author,isbn13,original_publication_year) VALUES(?,?,?,?)";

	if (sqlite3_open("Database.db", &database) == SQLITE_OK)
	{
		sqlite3_prepare(database, sql.c_str(), -1, &st, NULL);

		sqlite3_bind_text(st, 1, book_title.c_str(), book_title.length(), SQLITE_TRANSIENT);
		sqlite3_bind_text(st, 2, book_author.c_str(), book_author.length(), SQLITE_TRANSIENT);
		sqlite3_bind_text(st, 3, isbn13.c_str(), isbn13.length(), SQLITE_TRANSIENT);
		sqlite3_bind_int(st, 4, book_year);

		sqlite3_step(st);
		return true;

	}
	else
	{
		std::cout << "Failed to connect\n";
		return false;

	}

	sqlite3_close(database);
}

std::string Database::ShowBorrows(int userID)
{
	OpenDatabase("Database.db");
	Data results;
	std::string id = std::to_string(userID);
	if (this->GetQueryResult("SELECT borrows.book_id,books.title,books.authors,borrows.brought_date FROM borrows LEFT JOIN books ON borrows.book_id=books.id WHERE borrows.user_id='" + id + "';", results))
	{
		std::string response;
		if (results.rows > 0)
		{
			int count = 1;
			for (int i = 0; i < results.vector.size(); i++)
			{
				
				response += results.vector[i];
				if (count!=4)
				{
					response.append("|");
					count++;
				}
				else 
				{
					response.append("\n");
					count = 1;
				}
			}
		}
		return response;
	}
	else
	{
		return "GET_BORROWED_BOOKS | FAILED";
	}
}

std::string Database::DeleteUser(int userId)
{
	if (DeleteAllBorrowsOfUser(userId))
	{
		std::string user_id = std::to_string(userId);
		const std::string& query = "DELETE FROM users WHERE id='" + user_id + "'; ";

		status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
		if (status != SQLITE_OK) {
			logl("Could not execute query");
			sqlite3_free(dbError);
			return "ACC_DELETE | FAILED";
		}
		else
		{
			return "ACC_DELETE | OK";
		}
	}
	else
	{
		return "ACC_DELETE_BORROWS | FAILED";
	}
}

bool Database::DeleteAllBorrowsOfUser(int userId)
{
	std::string userId_s = std::to_string(userId);
	const std::string& query = "DELETE FROM borrows WHERE user_id='" + userId_s + "'; ";

	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		logl("Could not execute query");
		sqlite3_free(dbError);
		return false;
	}
	else
	{
		return true;
	}
}

std::string Database::DeleteBorrow(int userId, int bookId)
{
	std::string userId_s = std::to_string(userId);
	std::string bookId_s = std::to_string(bookId);
	const std::string& query = "DELETE FROM borrows WHERE user_id= '" + userId_s + "' AND book_id='" + bookId_s + "'";

	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		logl("Could not execute query");
		sqlite3_free(dbError);
		return "RETURN | FAILED";
	}
	else
	{
		return "RETURN | OK";
	}
}

bool Database::HasBorrows(int userId)
{
	std::string userId_s = std::to_string(userId);

	const std::string& query = "SELECT * FROM borrows WHERE user_id='" + userId_s + "'";

	status = sqlite3_exec(database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		//logl("No borrows!");
		sqlite3_free(dbError);
		return false;
	}
	else
	{
		return true;
	}
}

std::string Database::readBookById(int bookId)
{
	Data results;
	std::string bookId_s = std::to_string(bookId);

	if (GetQueryResult("SELECT books.title,books.authors FROM books WHERE books.id='" + bookId_s + "';", results))
	{
		if (results.rows > 0)
		{
			std::string response;
			response += results.vector[0];
			response += "|";
			response += results.vector[1];
			return response;
		}
	}

	return "READ_BOOK | FAILED";
}

bool Database::Has5BorrowsAlready(int userId)
{
	Data data;
	std::string userId_s = std::to_string(userId);

	GetQueryResult("SELECT COUNT (*) FROM borrows WHERE user_id='" + userId_s + "'", data);
	if (data.vector[0] == "5")
	{
		return true;
	}
	return false;
}

bool Database::NoMoreExamplesOfBook(int bookId)
{
	Data data;
	std::string bookId_s = std::to_string(bookId);

	GetQueryResult("SELECT COUNT (*) FROM borrows WHERE book_id='" + bookId_s + "'", data);
	if (data.vector[0] == "3")
	{
		return true;
	}
	return false;
}

std::string Database::getPreviewBook(int bookId)
{
	OpenDatabase("Database.db");
	Data results;
	std::string id = std::to_string(bookId);
	if (this->GetQueryResult("SELECT isbn,title,authors,image_url FROM books WHERE id='" + id + "';", results))
	{
		std::string response;
		if (results.rows > 0)
		{
			for (int i = 0; i < results.vector.size(); i++)
			{
				response += results.vector[i];
				
				response.append("|");
				
					
				
			}
		}
		return response;
	}
	else
	{
		return "GET_BORROWED_BOOKS | FAILED";
	}
}

std::string Database::ExtendBorrow(int userId, int bookId)
{
	std::string userId_s = std::to_string(userId);
	std::string bookId_s = std::to_string(bookId);
	const std::string& query = "UPDATE borrows SET brought_date=DATE(brought_date, '+14 day'),extensions=extensions+1 WHERE user_id='" + userId_s + "' AND book_id='" + bookId_s + "' ;";

	status = sqlite3_exec(this->database, query.c_str(), callback, 0, &dbError);
	if (status != SQLITE_OK) {
		logl("No borrows");
		sqlite3_free(dbError);
		return "EXTEND | FAILED";
	}
	else
	{
		return "EXTEND | OK";
	}
}
	


void Database::ConnectTables()
{
	ExecuteQuery("ALTER TABLE USERS ADD FOREIGN KEY (ID ) REFERENCES BORROWS (USER_ID );");
	ExecuteQuery("ALTER TABLE BOOKS ADD FOREIGN KEY (ID ) REFERENCES BORROWS (BOOK_ID );");
}

void Database::CloseDatabase() {
	sqlite3_close(database);
}

void Database::CreateVirtualTables()
{
	ExecuteQuery("CREATE VIRTUAL TABLE if not exists titlefts USING fts4(title text)");
	ExecuteQuery("CREATE VIRTUAL TABLE if not exists titlefts_terms USING fts4aux(titlefts)");
	

	ExecuteQuery("CREATE VIRTUAL TABLE if not exists authorfts USING fts4(author text)");
	ExecuteQuery("CREATE VIRTUAL TABLE if not exists authorfts_terms USING fts4aux(authorfts)");
	
	ExecuteQuery("CREATE VIRTUAL TABLE if not exists titleauthorspellfix USING spellfix1");
}

std::string Database::Spellfix(std::string searchTerm)
{
	Data result;
	if (this->GetQueryResult("SELECT word FROM titleauthorspellfix WHERE word MATCH '" + searchTerm + "' and top=1;", result))
	{	
		if (searchTerm != result.vector[0])
		{
			return result.vector[0];
		}
	}
	return searchTerm;
}

std::string Database::Search(std::string term)
{
	std::string fixedQuery = Spellfix(term);
	if (!fixedQuery.empty())
	{
		Data result;
		GetQueryResult("SELECT * FROM titlefts WHERE title MATCH '" + fixedQuery + "'", result);
		if (!result.vector.empty())
		{
			std::string getTitle;
			for (const auto &r : result.vector)
			{
				getTitle += GetBookByTitle(r);
			}
			return getTitle;
		}
		else
		{
			result.vector.clear();
			GetQueryResult("SELECT * FROM authorfts WHERE author MATCH '" + fixedQuery + "'", result);
			if (!result.vector.empty())
			{
				std::string getAuthor;
				for (const auto& r : result.vector)
				{
					getAuthor += GetBookByAuthor(r);
				}
				return getAuthor;
			}
			else
			{
				result.vector.clear();
				GetQueryResult("select book_id,title,authors from books where isbn = '" + fixedQuery + "'", result);
				if (!result.vector.empty())
				{
					std::string getISBN;
					for (const auto& r : result.vector)
					{
						getISBN += r;
						getISBN.append("|");
					}
					return getISBN;
				}
			}
		}
	}
	return "Book search - FAILED";
}

std::string Database::GetBookByTitle(std::string title)
{
	Data results;
	if (this->GetQueryResult("select id,title,authors from books where title='" + title + "'", results))
	{
		std::string response;
		if (results.rows > 0)
		{
			int count = 1;
			for (int i = 0; i < results.vector.size(); i++)
			{

				response += results.vector[i];
				if (count != 3)
				{
					response.append("|");
					count++;
				}
				else
				{
					response.append("\n");
					count = 1;
				}
			}
		}
		return response;
	}
	else
	{
		return "SEARCH - FAILED";
	}

}
std::string Database::GetBookByAuthor(std::string author)
{
	Data results;
	if (this->GetQueryResult("select id,title,authors from books where author='" + author + "'", results))
	{
		std::string response;
		if (results.rows > 0)
		{
			int count = 1;
			for (int i = 0; i < results.vector.size(); i++)
			{

				response += results.vector[i];
				if (count != 3)
				{
					response.append("|");
					count++;
				}
				else
				{
					response.append("\n");
					count = 1;
				}
			}
		}
		return response;
	}
	else
	{
		return "SEARCH - FAILED";
	}

}



static int callback(void* data, int argc, char** argv, char** azColName) {
	Data* d = (Data*)data;
	d->rows = argc;
	for (int i = 0; i < argc; i++) {
		if (data != nullptr) {
			d->vector.push_back((argv[i] ? argv[i] : "NULL"));
		}
	}
	return 0;
}

Database::~Database()
{
}
