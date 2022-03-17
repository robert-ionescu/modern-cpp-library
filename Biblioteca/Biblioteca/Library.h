#pragma once
#include <SFML/Network.hpp>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include"LoginState.h"
#include"TGUI/TGUI.hpp"

enum Requests
{
	LogInVerification,
	SearchVerification,
	BorrowedBooks,
	BorrowBook,
	ReturnBook,
	ReadBook,
	RegisterVerification,
	DeleteAccount,
	PreviewBook
};


class Library
{
private:
	//vars
	const std::string initWindowFile = "InitWindow.txt";

	sf::RenderWindow *window;

	sf::Clock deltaClock;
	float deltaTime;

	tgui::GuiSFML gui;

	//init
	void initWindow();

	//user id
	int userId;

	//server
	sf::TcpSocket socket;
	sf::Packet request;
	sf::Packet packet;
	bool isConnected;
public:
	Library();
	virtual ~Library();

	//functions

	void run();
	void loadLoginPageWidgets();
	bool runLoginPage();
	

	void loadRegisterPageWidgets();
	bool runRegisterPage();

	void loadMainMenuPageWidgets();
	void addBookToPanel(const char* bookImgSrc, const char* bookLeftPos, const char* buttonLeftPos,
		const char* bookCoverTopPos, const char* bookTextTopPos, const char* buttonTopPos, const int& bookId);
	void createSearchWindow(const std::string&);
	bool runMainMenuPage();
	
	void loadUserProfilePageWidgets();
	void returnBook(const tgui::String& bookId, tgui::ListBox::Ptr myBooksListBox);
	bool runUserProfilePage();
	
	void loadPreviewBookPageWidgets(int);
	bool runPreviewBookPage(int);

	void loadReadBookPageWidgets(int);
	bool runReadBookPage(int);

	//server funtions
	void connect(std::string address, unsigned short port);
	void receivePackets(sf::Packet& packet);
	void sendPacket(sf::Packet& packet);
	void makePacket(Requests request);
	

	
	void waitForRequest();
	bool waitForConfirmation();
};

