#include"Server.h"
#include<iostream>

Server::Server(unsigned short _listenPort)

	:listenPort(_listenPort),
	logs()
{
	//logs.EnableFileOutput("logspleasework2.txt");
	Logs::EnableFileOutput("logs.txt");

	Logs::Informational("Server Started");
	if (listener.listen(listenPort) != sf::Socket::Done)
	{
		Logs::Critical("Could not listen");
		throw("");
	}
	clients = std::make_shared<std::vector< std::shared_ptr<sf::TcpSocket>>>();

}

Server::~Server()
{
	for (int i = 0; i < this->clients.get()->size(); ++i)
	{
		std::shared_ptr<sf::TcpSocket> socket = clients.get()->at(i);
		this->disconnectClient(socket, i);
	}
}


void Server::connectClients(std::shared_ptr<std::vector< std::shared_ptr<sf::TcpSocket>>> clients)
{

	while (true)
	{
		std::shared_ptr<sf::TcpSocket> newClient = std::make_shared<sf::TcpSocket>();
		if (this->listener.accept(*newClient) == sf::Socket::Done)
		{
			newClient->setBlocking(false);
			clients.get()->push_back(newClient);
			Logs::Informational("New Client has connected!"); //log that a new client has connected


		}
		else
		{
			Logs::Critical("Connection has failed!"); //log that connection has failed
			break;
		}
	}
}

void Server::disconnectClient(std::shared_ptr<sf::TcpSocket> _socket, size_t position)
{
	Logs::Informational("A Client has disconencted"); //log that a client has been disconnected
	_socket->disconnect();
	clients.get()->erase(clients.get()->begin() + position);
}

void Server::receivePacket(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	packet.clear();
	while (true)
	{
		if (client->receive(packet) != sf::Socket::Disconnected)
		{
			if (packet.getDataSize() > 0)
			{
				
				std::string logMessage;
				logMessage.append("Client with Address ");
				logMessage.append(std::to_string(client->getRemotePort()));
				logMessage.append(" and port ");
				logMessage.append(std::to_string(client->getRemotePort()));
				logMessage.append(" has received data");
				Logs::Informational(logMessage);//log that current client received data;client->getRemoteAddress(), client->getRemotePort()
				break;
			}
		}
	}
	

}

void Server::sendPacket(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet,const std::string& message)
{
	packet.clear();
	packet << message;
	if (client.get()->send(packet) != sf::Socket::Done)
	{
		Logs::Critical("Data has not been sent!");//log that data was not sent;
	}

}

void Server::sendConfirmation(std::shared_ptr<sf::TcpSocket> client, bool confirmation)
{
	sf::Packet confirmationPacket;
	confirmationPacket << confirmation;
	if (client->send(confirmationPacket) != sf::Socket::Done)
	{
		Logs::Critical("Confirmation has not been sent!"); //log that confirmation was not sent;
	}
}

void Server::waitRequest(std::shared_ptr<sf::TcpSocket> client, sf::Packet& request)
{
	while (true)
	{
		if (client->receive(request) != sf::Socket::Disconnected)
		{
			if (request.getDataSize() > 0)
			{
				break;
			}
		}
	}
}

std::string Server::filterVerification(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet, Requests request)
{
	std::string message;
	switch (request)
	{
	case LogInVerification:
	{
		message = logInVerification(client, packet);
		break;
	}
	case SearchVerification:
	{
		message = search(client, packet);
		break;
	}
	case BorrowedBooks:
	{
		message = borrowedBooks(client, packet);
		break;
	}
	case BorrowBook:
	{
		message = borrowBook(client, packet);
		break;
	}
	case ReturnBook:
	{
		message = returnBook(client, packet);
		break;
	}
	case ReadBook:
	{
		message = readBook(client, packet);
		break;
	}
	case RegisterVerification:
	{
		message = registerVerification(client, packet);
		break;
	}
	case DeleteAccount:
	{
		message = deleteAccount(client, packet);
		break;
	}
	case PreviewBook:
	{
		message = previewBook(client, packet);
		break;
	}
	default:
	{
		break;
	}

	}
	return message;
}

Requests Server::identifyVerificationType(std::shared_ptr<sf::TcpSocket> client, sf::Packet& requestPacket)
{
	int value;
	requestPacket >> value;
	Requests request = static_cast<Requests>(value);
	sendConfirmation(client, true);
	return request;
}

std::string Server::logInVerification(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	std::string _user;
	std::string _pass;
	packet >> _user >> _pass;

	return database.VerifyUserLogIn(_user, _pass,database);
}

std::string Server::registerVerification(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	std::string _user;
	std::string _pass;
	packet >> _user >> _pass;

	return database.AddUser("", "", "", _user, _pass);
}

std::string Server::returnBook(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	int user_id, book_id;
	packet >> user_id >> book_id;
	return database.DeleteBorrow(user_id, book_id);
}

std::string Server::search(std::shared_ptr<sf::TcpSocket>, sf::Packet& packet)
{
	std::string text;
	packet >> text;
	return database.Search(text);
}

std::string Server::borrowedBooks(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	int user_id;
	packet >> user_id;
	return database.ShowBorrows(user_id);
}

std::string Server::readBook(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	int book_id;
	packet >> book_id;
	return database.readBookById(book_id);
}

std::string Server::previewBook(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	int book_id;
	packet >> book_id;
	return database.getPreviewBook(book_id);
}

std::string Server::borrowBook(std::shared_ptr<sf::TcpSocket> client, sf::Packet packet)
{
	int user_id;
	int book_id;
	packet >> user_id >> book_id;
	return database.AddBorrows(user_id, book_id, getCurrentDate(), getDateIn2Weeks());
}

std::string Server::deleteAccount(std::shared_ptr<sf::TcpSocket> client, sf::Packet& packet)
{
	int user_id;
	packet >> user_id;
	return database.DeleteUser(user_id);
}

std::string Server::getCurrentDate()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y");
	auto str = oss.str();
	return str;
}

std::string Server::getDateIn2Weeks()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	tm.tm_yday += 14;
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y");
	auto str = oss.str();
	return str;
}

void Server::managePackets(std::shared_ptr<sf::TcpSocket> client)
{
	sf::Packet requestPacket;
	sf::Packet packet;
	while (true)
	{
		requestPacket.clear();
		packet.clear();
		waitRequest(client, requestPacket);
		Requests request = identifyVerificationType(client, requestPacket);
		receivePacket(client, packet);
		std::string message=filterVerification(client, packet, request);
		sendPacket(client, packet,message);
		std::this_thread::sleep_for((std::chrono::milliseconds)100);
	}
}


void Server::run()
{
	std::thread connectionThread(&Server::connectClients, this, std::ref(clients));
	std::vector<std::thread> workers;
	if (clients.get()->size() == 0)
	{
		while (true)
		{
			if (clients.get()->size() != 0)
			{
				workers.push_back(std::thread(&Server::managePackets, this, std::ref(clients.get()->at(clients.get()->size() - 1))));
				break;
			}
		}
	}
	int count = clients.get()->size();
	while (true)
	{
		if (count != clients.get()->size())
		{
			workers.push_back(std::thread(&Server::managePackets, this, std::ref(clients.get()->at(clients.get()->size() - 1))));
			count = clients.get()->size();
		}

	}


}