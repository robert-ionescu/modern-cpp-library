#pragma once
#include<SFML/Network.hpp>
#include<vector>
#include"Logs.h"
#include<vector>
#include<thread>
#include <string>
#include "Database.h"
#include<memory>
#include <ctime>
#include<iomanip>


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

class Server
{
private:
	//Logs
	Logs logs;

	//receive data
	sf::TcpListener listener;
	unsigned short listenPort;



	//connections to clients
	std::shared_ptr<std::vector< std::shared_ptr<sf::TcpSocket>>> clients;

	//Database
	Database database;

public:
	Server(unsigned short _listenPort);
	~Server();
	
	//manage clients
	void connectClients(std::shared_ptr<std::vector< std::shared_ptr<sf::TcpSocket>>> clients);
	void disconnectClient(std::shared_ptr<sf::TcpSocket>, size_t position);

	//manage data
	void receivePacket(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	void sendPacket(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet,const std::string& message);
	void sendConfirmation(std::shared_ptr<sf::TcpSocket>, bool confirmation);
	void waitRequest(std::shared_ptr<sf::TcpSocket>, sf::Packet& request);
	std::string filterVerification(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet, Requests request);
	Requests identifyVerificationType(std::shared_ptr<sf::TcpSocket>, sf::Packet& request);

	std::string logInVerification(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string registerVerification(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string returnBook(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string search(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string borrowedBooks(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string readBook(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet);
	std::string borrowBook(std::shared_ptr<sf::TcpSocket>, sf::Packet packet);
	std::string previewBook(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet);
	std::string deleteAccount(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet);


	std::string getCurrentDate();
	std::string getDateIn2Weeks();

	void managePackets(std::shared_ptr<sf::TcpSocket>);
	void run();

};
