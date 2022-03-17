#include "Library.h"
#include <string>

//Initializers
void Library::initWindow()
{
	std::ifstream in(this->initWindowFile);

	std::string title = "None";
	sf::VideoMode window_bounds(0, 0);
	unsigned framerate_limit = 0;
	bool vertical_sync_enable = false;

	if (in.is_open())
	{
		std::getline(in, title);
		in >> window_bounds.width >> window_bounds.height;
		in >> framerate_limit;
		in >> vertical_sync_enable;
	}

	this->window = new sf::RenderWindow(window_bounds,title);
	this->window->setFramerateLimit(framerate_limit);
	this->window->setVerticalSyncEnabled(vertical_sync_enable);
}


//Constructors / Destructors
Library::Library()
{
	this->userId = -1;
	this->isConnected = false;


	this->initWindow();
	gui.setTarget(*this->window);
}

Library::~Library()
{
	delete this->window;
}


//Functions



//Get 'register' form input
void getRegisterInput(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password, tgui::EditBox::Ptr cpassword)
{
	std::cout << "Username: " << username->getText() << std::endl;
	std::cout << "Password: " << password->getText() << std::endl;
	std::cout << "Confirmed Password: " << cpassword->getText() << std::endl;
}

//Dynamic update of widget text size
void updateTextSize(tgui::GuiBase& gui)
{
	const float windowHeight = gui.getView().getRect().height;
	gui.setTextSize(static_cast<unsigned int>(0.02f * windowHeight));
}

void Library::loadLoginPageWidgets()
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	// Create the username edit box
	auto editBoxUsername = tgui::EditBox::create();
	editBoxUsername->setSize({ "35.67%", "5.5%" });
	editBoxUsername->setPosition({ "33.67%", "27.67%" });
	editBoxUsername->setDefaultText("Username");
	gui.add(editBoxUsername);

	// Create the password edit box
	auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
	editBoxPassword->setPosition({ "33.67%", "36.6%" });
	editBoxPassword->setPasswordCharacter('*');
	editBoxPassword->setDefaultText("Password");
	gui.add(editBoxPassword);

	//Login error message
	auto errorMessage = tgui::Label::create();
	errorMessage->setPosition({ tgui::bindLeft(editBoxPassword), tgui::bindBottom(editBoxPassword) + 5 });
	errorMessage->setAutoSize(true);
	errorMessage->setMaximumTextWidth(editBoxPassword->getSize().x);
	gui.add(errorMessage);

	// Create the login button
	auto logButton = tgui::Button::create("Log In");
	logButton->setSize({ "17%", "5.67%" });
	logButton->setPosition({ "33.67%", tgui::bindBottom(errorMessage) + 10 });
	gui.add(logButton);

	//Create the register button
	auto registerButton = tgui::Button::create("Register");
	registerButton->setOrigin(1.0f, 0.0f);
	registerButton->setSize({ "17%", "5.67%" });
	registerButton->setPosition({ "69.34%", tgui::bindTop(logButton) });
	gui.add(registerButton);
	bool loginValidated = false;

	logButton->onPress([this,editBoxPassword, editBoxUsername, &loginValidated, errorMessage]
		{
			sf::String user(editBoxUsername->getText());
			sf::String pass(editBoxPassword->getText());
			std::string password = pass;
			std::string username = user;
			this->request.clear();
			this->makePacket(Requests::LogInVerification);
			this->sendPacket(this->request);
			if (this->waitForConfirmation())
			{
				this->packet.clear();
				this->packet << username << password;
				//this->makePacket(Requests::LogInVerification);
				this->sendPacket(this->packet);
				this->receivePackets(this->packet);
				std::string response;
				packet >> response;
				std::cout << response;
				if (response != "LOGIN | FAILED")
				{
					loginValidated = true;
					userId =std::stoi(response);
					runMainMenuPage();
				}
				else if (response == "LOGIN | FAILED")
				{
					errorMessage->setText("Invalid credentials. (Are you registered?)");
				
				}
			}	
		});

	logButton->onPress([this,loginValidated] { if(loginValidated) runMainMenuPage(); });

	registerButton->onPress([this] {  runRegisterPage(); });
}

void Library::loadRegisterPageWidgets()
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	// Create the username edit box
	auto editBoxUsername = tgui::EditBox::create();
	editBoxUsername->setSize({ "35.67%", "5.5%" });
	editBoxUsername->setPosition({ "33.67%", "27.67%" });
	editBoxUsername->setDefaultText("Username");
	gui.add(editBoxUsername);

	// Create the password edit box
	auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
	editBoxPassword->setPosition({ "33.67%", "36.6%" });
	editBoxPassword->setPasswordCharacter('*');
	editBoxPassword->setDefaultText("Password");
	gui.add(editBoxPassword);

	//Create the 'confirm password' edit box
	auto editBoxCPassword = tgui::EditBox::copy(editBoxUsername);
	editBoxCPassword->setPosition({ "33.67%", "45.53%" });
	editBoxCPassword->setPasswordCharacter('*');
	editBoxCPassword->setDefaultText("Confirm Password");
	gui.add(editBoxCPassword);

	//Create the register button
	auto regButton = tgui::Button::create("Register");
	regButton->setSize({ "15%", "5.67%" });
	regButton->setPosition({ "43.67%", "55%" });
	gui.add(regButton);
	
	//Create the back to login page button
	auto backButton = tgui::Button::create("Back");
	backButton->setSize({ "4.53125%", "2.75%" });
	backButton->setPosition({ "1.5625%", "2.5%" });
	gui.add(backButton);
	bool validation = false;

	regButton->onPress([this, editBoxUsername, editBoxPassword, editBoxCPassword, regButton, &validation]
		{
			sf::String user(editBoxUsername->getText());
			sf::String pass(editBoxPassword->getText());
			sf::String cpass(editBoxCPassword->getText());
			std::string password = pass;
			std::string username = user;
			std::string cpassword = cpass;
			std::string errorMessage = "";

			auto passErrorWindow = tgui::ChildWindow::create();
			passErrorWindow->setSize({ "47%", "40%" });
			passErrorWindow->setOrigin(0.5f, 0.5f);
			passErrorWindow->setPosition({ "50%", "45%" });

			auto passErrorWindowText = tgui::Label::create(errorMessage);
			auto passErrorOKButton = tgui::Button::create("OK");

			passErrorWindow->add(passErrorWindowText);
			passErrorWindow->add(passErrorOKButton);

			passErrorWindowText->setAutoSize(true);
			passErrorWindowText->setOrigin(0.5f, 0.5f);
			passErrorWindowText->setPosition({ "50%", "50%" });

			passErrorOKButton->setSize({ "15.25%", "8.66%" });
			passErrorOKButton->setOrigin(0.5f, 0.5f);
			passErrorOKButton->setPosition({ "50%", "85%" });

			
			passErrorOKButton->onPress([this, passErrorWindow] { gui.remove(passErrorWindow); });

			bool usernameError = false, passError = false, passMatchError = false;
			if (username.size() <= 4 || username.size() > 16)
			{
				usernameError = true;
			}
			if (password.size() <= 6 || username.size() > 32)
			{
				passError = true;
			}
			if (password != cpassword)
			{
				passMatchError = true;
			}
			if (usernameError || passError || passMatchError)
			{
				validation = false;
				

				
				if (usernameError)
				{
					errorMessage.append("Username length must be between four and sixteen characters.\n");
				}
				if (passError)
				{
					errorMessage.append("Password length must be between six and thirty-two characters.\n");
				}
				if (passMatchError)
				{
					errorMessage.append("The passwords do not match.");
				}
				passErrorWindowText->setText(errorMessage);
				gui.add(passErrorWindow);
				
			}
			else
			{
				this->request.clear();
				this->makePacket(Requests::RegisterVerification);
				this->sendPacket(this->request);
				std::string response;
				if (this->waitForConfirmation())
				{
					this->packet.clear();
					this->packet << username << password;
					this->sendPacket(this->packet);
					this->receivePackets(this->packet);
					
					packet >> response;
					std::cout << response;
					
				}
				if (response == "REGISTER | OK")
				{
					validation = true;
					runLoginPage();
				}
				else
				{
					errorMessage.append("The username is already taken");
					passErrorWindowText->setText(errorMessage);
					gui.add(passErrorWindow);
				}
			}
		});

	regButton->onPress([this, validation] { if(validation) runLoginPage(); });

	backButton->onPress([this] { runLoginPage(); });
}

void Library::addBookToPanel(const char* bookImgSrc, const char* bookLeftPos, const char* buttonLeftPos,
	const char* bookCoverTopPos, const char* bookTextTopPos, const char* buttonTopPos, const int& bookId)
{
	//Book cover
	auto bookCover = tgui::Picture::create(tgui::String(bookImgSrc));
	bookCover->setSize({ "14.5%", "28.333333333333332%" });
	bookCover->setPosition({ tgui::String(bookLeftPos), tgui::String(bookCoverTopPos) });
	gui.add(bookCover);

	//View button
	auto viewBookButton = tgui::Button::create("View Book");
	viewBookButton->setSize({ "7.2%", "2.75%" });
	viewBookButton->setPosition({ tgui::String(buttonLeftPos), tgui::String(buttonTopPos) });
	viewBookButton->onPress([this, bookId] {runPreviewBookPage(bookId); });
	gui.add(viewBookButton);
}

void Library::createSearchWindow(const std::string& searchInput)
{
	auto searchWindow = tgui::ChildWindow::create();
	searchWindow->setSize({ "75%", "75%" });
	searchWindow->setOrigin(0.5f, 0.5f);
	searchWindow->setPosition({ "50%", "50%" });

	auto searchResultsText = tgui::Label::create("Search results: ");
	searchResultsText->setAutoSize(true);
	searchResultsText->setPosition({ "5%", "5%" });
	searchWindow->add(searchResultsText);

	auto searchResultsBox = tgui::ListBox::create();
	searchWindow->add(searchResultsBox);

	searchResultsBox->setPosition({ tgui::bindLeft(searchResultsText), "11.5%" });
	searchResultsBox->setSize({ "45.3125%", "71.875%" });
	searchResultsBox->setItemHeight(32);

	//add books to the search box
	this->request.clear();
	this->makePacket(Requests::SearchVerification);
	this->sendPacket(this->request);
	if (this->waitForConfirmation())
	{
		this->packet.clear();
		packet << searchInput;
		this->sendPacket(this->packet);
		this->receivePackets(this->packet);
		std::string response;
		packet >> response;
		std::cout << response;
		if (response.size())
		{
			int index = 0;
			std::string bookId = "", bookTitle = "", bookAuthor = "";
			for (int i = 0; i < response.length(); i++)
			{
				if (index == 0)
				{
					if (response[i] != '|')
					{
						bookId.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else if (index == 1)
				{
					if (response[i] != '|')
					{
						bookAuthor.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}

				else
				{
					if (response[i] != '\n')
					{
						bookTitle.push_back(response[i]);
					}
					else
					{
						std::string listedBook = bookAuthor + " - " + "\"" + bookTitle + "\"";
						searchResultsBox->addItem(listedBook, bookId);
						index = 0;
						bookId = ""; bookTitle = ""; bookAuthor = "";
					}
				}
			}
		}
	}

	//Preview book button (takes the user to the preview page)
	auto previewBookButton = tgui::Button::create("View Book");
	previewBookButton->setSize({ "12%", "3.25%" });
	previewBookButton->setPosition({ tgui::bindLeft(searchResultsBox), tgui::bindBottom(searchResultsBox) + 20 });
	searchWindow->add(previewBookButton);
	previewBookButton->onPress([this, searchResultsBox]
		{
			sf::String id(searchResultsBox->getSelectedItemId());
			std::string book_id = id;
			if (book_id.size())
			{
				int b_id = std::stoi(book_id);
				runPreviewBookPage(b_id);
			}
		});

	//Borrow book button
	auto borrowBookButton = tgui::Button::create("Borrow Book");
	borrowBookButton->setSize({ "12%", "3.25%" });
	borrowBookButton->setPosition({ tgui::bindRight(previewBookButton) + 20, tgui::bindTop(previewBookButton) });
	searchWindow->add(borrowBookButton);

	//borrow the selected book
	borrowBookButton->onPress([this, searchResultsBox,searchWindow,borrowBookButton]
		{
			sf::String id(searchResultsBox->getSelectedItemId());
			std::string book_id = id;
			if (book_id.size())
			{
				int b_id = std::stoi(book_id);
				auto borrowSuccesLabel = tgui::Label::create("Book borrowed successfuly.");
				borrowSuccesLabel->setAutoSize(true);
				borrowSuccesLabel->setPosition({ tgui::bindRight(borrowBookButton) + 20, tgui::bindTop(borrowBookButton) });

				this->request.clear();
				this->makePacket(Requests::BorrowBook);
				this->sendPacket(this->request);
				if (this->waitForConfirmation())
				{
					this->packet.clear();
					packet << userId << b_id;
					this->sendPacket(this->packet);
					this->receivePackets(this->packet);
					std::string response;
					packet >> response;
					std::cout << response;
					if (response == "BORROW | OK")
					{
						searchWindow->add(borrowSuccesLabel);
					}
					else
					{
						borrowSuccesLabel->setText("Book borrow failed.");
						searchWindow->add(borrowSuccesLabel);
					}

				}
			}
		});
	
	gui.add(searchWindow);
}

void Library::loadMainMenuPageWidgets()
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	// Create the "My Profile" button
	auto myProfileButton = tgui::Button::create("My Profile");
	myProfileButton->setSize({ "7.8125%", "3.25%" });
	myProfileButton->setPosition({ "12.5%", "3.125%" });
	gui.add(myProfileButton);
	myProfileButton->onPress([this] { runUserProfilePage(); });

	// Create the search edit box
	auto editBoxSearch = tgui::EditBox::create();
	editBoxSearch->setSize({ "12.5%", "3.25%" });
	editBoxSearch->setPosition({ "68.75%", "3.125%" });
	editBoxSearch->setDefaultText("Search...");
	gui.add(editBoxSearch);

	//Create the search button
	auto searchButton = tgui::Button::create("Search");
	searchButton->setSize({ "4.84375%", "3.25%" });
	searchButton->setPosition({ "82.8125%", "3.125%" });
	gui.add(searchButton);

	//Create the books panel
	auto booksPanel = tgui::ScrollablePanel::create();
	booksPanel->setSize({ "75%", "75%" });
	booksPanel->setPosition({ "12.5%", "12.5%" });
	gui.add(booksPanel);

	//Adding books to the panel

	//First row

	addBookToPanel("book_covers/04425019_hunger-games.jpg", "17.1875%", "20.5%", "17.5%", "40%", "47%", 1);

	addBookToPanel("book_covers/11031407.jpg", "42.95%", "46.2625%", "17.5%", "40%", "47%", 4);

	addBookToPanel("book_covers/15369017.jpg", "66.40625%", "70%", "17.5%", "40%", "47%", 5);

	//Second row

	addBookToPanel("book_covers/81EbEWM54ML.jpg", "17.1875%", "20.5%", "52.5%", "75%", "82%", 18);

	addBookToPanel("book_covers/9780141355078.jpg",  "42.95%", "46.2625%", "52.5%", "75%", "82%", 6);

	addBookToPanel("book_covers/Pride_and_Prejudice_Cover.jpg", "66.40625%", "70%", "52.5%", "75%", "82%", 10);

	//Search

	//get search box input

	//creates the search child window
	searchButton->onPress([this, editBoxSearch] {
		sf::String searchText(editBoxSearch->getText());
		std::string search = searchText;
		createSearchWindow(search);
		});
}

void Library::loadReadBookPageWidgets(int bookId)
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	//Scrollable book page
	auto pagePanel = tgui::ScrollablePanel::create();
	pagePanel->setSize({ "100%", "100%" });
	pagePanel->setPosition({ "0%", "0%" });
	pagePanel->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	pagePanel->add(picture);

	std::string title, author = "by ";
	this->request.clear();
	this->makePacket(Requests::ReadBook);
	this->sendPacket(this->request);
	if (this->waitForConfirmation())
	{
		this->packet.clear();
		packet << bookId;
		this->sendPacket(this->packet);
		this->receivePackets(this->packet);
		std::string response;
		packet >> response;
		std::cout << response;
		if (response != "READ_BOOK | FAILED")
		{
			int index = 0;
			for (int i = 0; i < response.length(); i++)
			{
				if (index == 0)
				{
					if (response[i] != '|')
						title.push_back(response[i]);
					else
						index++;
				}
				else
				{
					author.push_back(response[i]);
				}
			}
		}
		else
		{
			std::cout << "READ_BOOK | FAILED";
		}
	}
	//Page title (book title)
	auto bookTitle = tgui::Label::create(title);
	bookTitle->setTextSize(32);
	bookTitle->setAutoSize(true);
	bookTitle->setOrigin(0.5f, 0.5f);
	bookTitle->setPosition({ "50%", "6.25%" });
	pagePanel->add(bookTitle);

	//Book author label
	auto bookAuthor = tgui::Label::create(author);
	bookAuthor->setTextSize(22);
	bookAuthor->setAutoSize(true);
	bookAuthor->setOrigin(0.5f, 0.5f);
	bookAuthor->setPosition({ "50%", "10%" });
	pagePanel->add(bookAuthor);
	//Book content text
	std::string firstParagraph("Lorem ipsum dolor sit amet, consectetur adipiscing elit.Vestibulum sed nibh gravida, efficitur massa vitae, bibendum ligula.Suspendisse sed ex eu nisi sollicitudin facilisis.Ut semper auctor augue non porttitor.Nam id eros tempus augue luctus ullamcorper.Proin sit amet tempor diam.Nullam tempus vulputate tortor eget molestie.Fusce eu malesuada metus.Nulla placerat mauris eu tortor pellentesque posuere.Mauris fringilla convallis urna, hendrerit lacinia arcu maximus at. Vivamus condimentum lorem dui, ut aliquam mi congue sed.Phasellus luctus interdum nunc, ac elementum tellus faucibus in.Fusce egestas justo magna, id convallis diam semper non.Quisque ac nisi dignissim, iaculis mauris elementum, faucibus urna.Nullam quis semper ante, in vehicula eros.Aliquam feugiat ligula id urna pellentesque efficitur.\n");
	std::string secondParagraph("\nMorbi efficitur sapien in dolor egestas accumsan.Cras facilisis lobortis nunc scelerisque vestibulum.In cursus sem eget molestie feugiat.In non felis ut tellus vehicula eleifend.Suspendisse tincidunt, nisi eu varius molestie, nisl quam fermentum ante, ut luctus eros nibh sodales enim.Cras scelerisque libero suscipit lorem aliquam tempus.Etiam vel vulputate erat.Fusce non consequat nibh.Nulla finibus justo a convallis venenatis.Morbi aliquet tristique purus, ac convallis ex semper eu.Maecenas mollis dictum consequat.Vestibulum tincidunt lacus ut ullamcorper condimentum.Vivamus pretium tempor nunc sed hendrerit.Maecenas tortor urna, tincidunt eu nulla sit amet, consectetur ornare est.Suspendisse fringilla diam at aliquam ornare.\n");
	std::string thirdParagraph("\nPraesent maximus purus et suscipit pharetra. Fusce egestas tortor eu viverra tempus. Cras nec dui eu mauris consequat rutrum. In consequat id magna quis ultricies. Vivamus tincidunt odio a molestie dictum. Nullam ac risus sed neque ullamcorper aliquam. Maecenas sed porta elit, quis posuere enim. Nunc rutrum consectetur lacinia. Ut enim metus, pretium eget gravida at, mattis id nisi. Aenean in quam ac augue faucibus molestie nec at erat. Nulla luctus cursus lobortis. Donec at massa id arcu fermentum gravida ac ac dui. Nullam eros augue, ullamcorper id dapibus eu, vehicula ac magna. Praesent varius eleifend erat, vel malesuada orci consectetur eu.");
	auto bookContent = tgui::Label::create(firstParagraph + secondParagraph + thirdParagraph);
	bookContent->setAutoSize(true);
	bookContent->setMaximumTextWidth(1150);
	bookContent->setOrigin(0.5f, 0.5f);
	bookContent->setPosition({ "50%", "50%" });
	pagePanel->add(bookContent);

	//Back to user page button
	auto backButton = tgui::Button::create("Back");
	backButton->setSize({ "4.53125%", "2.75%" });
	backButton->setPosition({ "1.5625%", "2.5%" });
	pagePanel->add(backButton);

	gui.add(pagePanel);

	backButton->onPress([this] {runUserProfilePage(); });
}

void Library::returnBook(const tgui::String& id, tgui::ListBox::Ptr myBooksListBox)
{
	//Return book confirmation window
	auto returnBookWindow = tgui::ChildWindow::create();
	returnBookWindow->setSize({ "31.25%", "37.5%" });
	returnBookWindow->setOrigin(0.5f, 0.5f);
	returnBookWindow->setPosition({ "50%", "50%" });

	auto returnBookWindowText = tgui::Label::create("Are you sure you want to return this book?");
	auto returnBookYesButton = tgui::Button::create("Yes");
	auto returnBookCancelButton = tgui::Button::create("Cancel");
	returnBookWindow->add(returnBookWindowText);
	returnBookWindow->add(returnBookYesButton);
	returnBookWindow->add(returnBookCancelButton);

	returnBookWindowText->setAutoSize(true);
	returnBookWindowText->setOrigin(0.5f, 0.5f);
	returnBookWindowText->setPosition({ "50%", "50%" });

	returnBookYesButton->setSize({ "15.25%", "8.66%" });
	returnBookYesButton->setPosition({ "35%", "85%" });

	gui.add(returnBookWindow);

	//delete the book form user's borrowed books list using the id passed as function parameter (backend) - must be implemented
	
	//delete the book from the list
	returnBookYesButton->onPress([this, id, returnBookWindow, myBooksListBox]
		{	
			sf::String b_id(id);
			std::string book_id = b_id;
			int bookId = std::stoi(book_id);
			this->request.clear();
			this->makePacket(Requests::ReturnBook);
			this->sendPacket(this->request);
			if (this->waitForConfirmation())
			{
				this->packet.clear();
				packet << userId << bookId;
				this->sendPacket(this->packet);
				this->receivePackets(this->packet);
				std::string response;
				packet >> response;
				std::cout << response;
				if (response == "RETURN | OK")
				{
					myBooksListBox->removeItemById(id);
					gui.remove(returnBookWindow);
				}
				else
					std::cout << "\nRETURN | FAILED";
			}
		});

	returnBookCancelButton->setSize({ "15.25%", "8.66%" });
	returnBookCancelButton->setPosition({ tgui::bindRight(returnBookYesButton) + 15, "85%" });
	returnBookCancelButton->onPress([this, returnBookWindow] {gui.remove(returnBookWindow); });
}

void Library::loadUserProfilePageWidgets()
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	//Back to menu button
	auto backButton = tgui::Button::create("Back");
	backButton->setSize({ "4.53125%", "2.75%" });
	backButton->setPosition({ "1.5625%", "2.5%" });
	gui.add(backButton);

	//'my books' label
	auto myBooksLabel = tgui::Label::create("My Books");
	myBooksLabel->setPosition({ tgui::bindRight(backButton) + 15, "32.75%" });
	myBooksLabel->setTextSize(24);
	gui.add(myBooksLabel);

	//user's book list
	auto myBooksListBox = tgui::ListBox::create();
	myBooksListBox->setPosition({ tgui::bindLeft(myBooksLabel), "37.5%" });
	myBooksListBox->setSize({ "45.3125%", "21.875%" });
	myBooksListBox->setItemHeight(32);
	//add books
	//example: content + id (actual id from the db)
	this->request.clear();
	this->makePacket(Requests::BorrowedBooks);
	this->sendPacket(this->request);
	if (this->waitForConfirmation())
	{
		this->packet.clear();
		packet << userId;
		this->sendPacket(this->packet);
		this->receivePackets(this->packet);
		std::string response;
		packet >> response;
		std::cout << response;
		if (response != "GET_BORROWED_BOOKS | FAILED")
		{
			int index = 0;
			std::string bookId = "", bookTitle = "", bookAuthor = "", bookReturnDate = "";
			for (int i = 0; i < response.length(); i++)
			{
				if (index == 0)
				{
					if (response[i] != '|')
					{
						bookId.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else if (index == 1)
				{
					if (response[i] != '|')
					{
						bookAuthor.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else if (index == 2)
				{
					if (response[i] != '|')
					{
						bookTitle.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else
				{
					if (response[i] != '\n')
					{
						bookReturnDate.push_back(response[i]);
					}
					else
					{
						std::string listedBook = bookAuthor + " - " + "\"" + bookTitle + "\" - termen " + bookReturnDate;
						myBooksListBox->addItem(listedBook, bookId);
						index = 0;
						bookId = ""; bookTitle = ""; bookAuthor = ""; bookReturnDate = "";
					}
				}
			}
		}
		else
		{
			std::cout << "\nGET_BORROWED_BOOKS | FAILED";
		}
	}

	gui.add(myBooksListBox);

	//Read book button (takes the user to the reading page)
	auto readButton = tgui::Button::create("Read Book");
	readButton->setSize({ "8%", "3.25%" });
	readButton->setPosition({ tgui::bindLeft(myBooksListBox), tgui::bindBottom(myBooksListBox) + 20 });
	gui.add(readButton);
	readButton->onPress([this, myBooksListBox]
		{
			sf::String id(myBooksListBox->getSelectedItemId());
			std::string book_id = id;
			if (book_id.size())
			{
				int b_id = std::stoi(book_id);
				runReadBookPage(b_id);
			}
		});
	
	//Return book button
	auto returnBookButton = tgui::Button::create("Return Book");
	returnBookButton->setSize({ "9%", "3.25%" });
	returnBookButton->setPosition({ tgui::bindRight(readButton) + 20, tgui::bindTop(readButton) });
	gui.add(returnBookButton);

	returnBookButton->onPress([this, myBooksListBox]
		{
			tgui::String id = myBooksListBox->getSelectedItemId();
			if (id.size())
			{
				returnBook(id, myBooksListBox);
			}
		});


	

	//Create the logout button
	auto logoutButton = tgui::Button::create("Log Out");
	logoutButton->setSize({ "6%", "3.25%" });
	logoutButton->setPosition({ "80%", "2.5%" });
	gui.add(logoutButton);

	//Create the logout confirmation window
	auto logoutWindow = tgui::ChildWindow::create();
	logoutWindow->setSize({ "31.25%", "37.5%" });
	logoutWindow->setOrigin(0.5f, 0.5f);
	logoutWindow->setPosition({ "50%", "50%" });

	auto logoutWindowText = tgui::Label::create("Are you sure you want to log out?");
	auto logoutYesButton = tgui::Button::create("Yes");
	auto logoutCancelButton = tgui::Button::create("Cancel");
	logoutWindow->add(logoutWindowText);
	logoutWindow->add(logoutYesButton);
	logoutWindow->add(logoutCancelButton);

	logoutWindowText->setAutoSize(true);
	logoutWindowText->setOrigin(0.5f, 0.5f);
	logoutWindowText->setPosition({ "50%", "50%" });
	
	logoutYesButton->setSize({ "15.25%", "8.66%" });
	logoutYesButton->setPosition({ "35%", "85%" });

	//logs the user out and returns the user to the login page

	//return to login page
	logoutYesButton->onPress([this] {userId = -1; runLoginPage(); });

	logoutCancelButton->setSize({ "15.25%", "8.66%" });
	logoutCancelButton->setPosition({ tgui::bindRight(logoutYesButton) + 15, "85%" });
	logoutCancelButton->onPress([this, logoutWindow] {gui.remove(logoutWindow); });

	//Create the delete account button
	auto deleteAccButton = tgui::Button::create("Delete account");
	deleteAccButton->setSize({ "10%", "3.25%" });
	deleteAccButton->setPosition({ "87%", "2.5%" });
	gui.add(deleteAccButton);

	//Create the delete account confirmation window
	auto deleteAccWindow = tgui::ChildWindow::create();
	deleteAccWindow->setSize({ "31.25%", "37.5%" });
	deleteAccWindow->setOrigin(0.5f, 0.5f);
	deleteAccWindow->setPosition({ "50%", "50%" });

	auto deleteAccWindowText = tgui::Label::create("Are you sure you want to delete this account?");
	auto deleteAccYesButton = tgui::Button::create("Yes");
	auto deleteAccCancelButton = tgui::Button::create("Cancel");
	deleteAccWindow->add(deleteAccWindowText);
	deleteAccWindow->add(deleteAccYesButton);
	deleteAccWindow->add(deleteAccCancelButton);

	deleteAccWindowText->setAutoSize(true);
	deleteAccWindowText->setOrigin(0.5f, 0.5f);
	deleteAccWindowText->setPosition({ "50%", "50%" });

	deleteAccYesButton->setSize({ "15.25%", "8.66%" });
	deleteAccYesButton->setPosition({ "35%", "85%" });
	
	//delete account and return to login page
	deleteAccYesButton->onPress([this] {
		this->request.clear();
		this->makePacket(Requests::DeleteAccount);
		this->sendPacket(this->request);
		if (this->waitForConfirmation())
		{
			this->packet.clear();
			packet << userId;
			this->sendPacket(this->packet);
			this->receivePackets(this->packet);
			std::string response;
			packet >> response;
			std::cout << response;
			if (response == "ACC_DELETE | OK")
			{
				runLoginPage();
			}
			else
				std::cout << "\nACC_DELETE | FAILED";
		}
		});

	deleteAccCancelButton->setSize({ "15.25%", "8.66%" });
	deleteAccCancelButton->setPosition({ tgui::bindRight(deleteAccYesButton) + 15, "85%" });
	deleteAccCancelButton->onPress([this, deleteAccWindow] {gui.remove(deleteAccWindow); });

	backButton->onPress([this] { runMainMenuPage(); });

	logoutButton->onPress([this, logoutWindow]
		{ gui.add(logoutWindow); });
	deleteAccButton->onPress([this, deleteAccWindow]
		{ gui.add(deleteAccWindow); });
}

void parseURL(const std::string& url_s, std::string& host_, std::string& path_)
{
	std::string protocol_, query__;
	const std::string prot_end("://");
	std::string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
		prot_end.begin(), prot_end.end());
	protocol_.reserve(distance(url_s.begin(), prot_i));
	transform(url_s.begin(), prot_i,
		back_inserter(protocol_),
		std::ptr_fun<int, int>(tolower)); // protocol is icase
	if (prot_i == url_s.end())
		return;
	advance(prot_i, prot_end.length());
	std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
	host_.reserve(distance(prot_i, path_i));
	transform(prot_i, path_i,
		back_inserter(host_),
		std::ptr_fun<int, int>(tolower)); // host is icase
	std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
	path_.assign(path_i, query_i);
}

void Library::loadPreviewBookPageWidgets(int bookId)
{
	gui.removeAllWidgets();
	updateTextSize(gui);
	gui.onViewChange([this] { updateTextSize(gui); });

	auto picture = tgui::Picture::create("main_background.png");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	//get book data by id
	std::string ISBN = "", Title = "", Author = "", ImageURL = "";
	this->request.clear();
	this->makePacket(Requests::PreviewBook);
	this->sendPacket(this->request);
	if (this->waitForConfirmation())
	{
		this->packet.clear();
		packet << bookId;
		this->sendPacket(this->packet);
		this->receivePackets(this->packet);
		std::string response;
		packet >> response;
		std::cout << response;
		if (response != "GET_BORROWED_BOOKS | FAILED")
		{
			int index = 0;
			
			for (int i = 0; i < response.length(); i++)
			{
				if (index == 0)
				{
					if (response[i] != '|')
					{
						ISBN.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else if (index == 1)
				{
					if (response[i] != '|')
					{
						Title.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else if (index == 2)
				{
					if (response[i] != '|')
					{
						Author.push_back(response[i]);
					}
					else
					{
						index++;
					}
				}
				else
				{
					if (response[i] != '|')
					{
						ImageURL.push_back(response[i]);
					}
				}
			}
		}
		else
			std::cout << "\nget_borrowed | fail";
	}

	//download image
	std::string host, path;
	parseURL(ImageURL, host, path);
	sf::Http http(host);
	sf::Http::Request req(path);
	sf::Http::Response image_res = http.sendRequest(req);

	const std::string& body = image_res.getBody();
	std::ofstream file("cover.jpg", std::ios::binary);
	file.write(body.c_str(), body.size());
	file.close();

	//Picture of the book cover
	auto bookCover = tgui::Picture::create("cover.jpg");
	bookCover->setSize({ "14.84375%", "30%" });
	bookCover->setPosition({ "5.46875%", "5%" });
	gui.add(bookCover);

	//Book ISBN Label
	auto bookISBN = tgui::Label::create("ISBN: " + ISBN);
	bookISBN->setTextSize(26);
	bookISBN->setAutoSize(true);
	bookISBN->setPosition({ "28.125%", "5%" });
	gui.add(bookISBN);

	//Book Title Label
	auto bookTitle = tgui::Label::create(Title);
	bookTitle->setTextSize(26);
	bookTitle->setAutoSize(true);
	bookTitle->setPosition({ "28.125%", "10%" });
	gui.add(bookTitle);

	//Book Author Label
	auto bookAuthor = tgui::Label::create("by " + Author);
	bookAuthor->setTextSize(24);
	bookAuthor->setAutoSize(true);
	bookAuthor->setPosition({ "28.125%", "15%" });
	gui.add(bookAuthor);

	//Book content
	std::string firstParagraph("Content Preview:\n\nLorem ipsum dolor sit amet, consectetur adipiscing elit.Vestibulum sed nibh gravida, efficitur massa vitae, bibendum ligula.Suspendisse sed ex eu nisi sollicitudin facilisis.Ut semper auctor augue non porttitor.Nam id eros tempus augue luctus ullamcorper.Proin sit amet tempor diam.Nullam tempus vulputate tortor eget molestie.Fusce eu malesuada metus.Nulla placerat mauris eu tortor pellentesque posuere.Mauris fringilla convallis urna, hendrerit lacinia arcu maximus at. Vivamus condimentum lorem dui, ut aliquam mi congue sed.Phasellus luctus interdum nunc, ac elementum tellus faucibus in.Fusce egestas justo magna, id convallis diam semper non.Quisque ac nisi dignissim, iaculis mauris elementum, faucibus urna.Nullam quis semper ante, in vehicula eros.Aliquam feugiat ligula id urna pellentesque efficitur.\n");
	std::string secondParagraph("\nMorbi efficitur sapien in dolor egestas accumsan.Cras facilisis lobortis nunc scelerisque vestibulum.In cursus sem eget molestie feugiat.In non felis ut tellus vehicula eleifend.Suspendisse tincidunt, nisi eu varius molestie, nisl quam fermentum ante, ut luctus eros nibh sodales enim.Cras scelerisque libero suscipit lorem aliquam tempus.Etiam vel vulputate erat.Fusce non consequat nibh.Nulla finibus justo a convallis venenatis.Morbi aliquet tristique purus, ac convallis ex semper eu.Maecenas mollis dictum consequat.Vestibulum tincidunt lacus ut ullamcorper condimentum.Vivamus pretium tempor nunc sed hendrerit.Maecenas tortor urna, tincidunt eu nulla sit amet, consectetur ornare est.Suspendisse fringilla diam at aliquam ornare.\n");
	auto bookContentPreviewText = tgui::Label::create(firstParagraph + secondParagraph);
	bookContentPreviewText->setAutoSize(true);
	bookContentPreviewText->setMaximumTextWidth(1150);
	bookContentPreviewText->setPosition({ tgui::bindLeft(bookCover),  tgui::bindBottom(bookCover) + 100});
	gui.add(bookContentPreviewText);

	//Button for borrowing the book
	auto borrowButton = tgui::Button::create("Borrow");
	borrowButton->setSize({ "9.25%", "3.25%" });
	borrowButton->setPosition({ "87%", "2.5%" });
	gui.add(borrowButton);

	//Back to menu button
	auto backButton = tgui::Button::create("Back");
	backButton->setSize({ "3.75%", "2.75%" });
	backButton->setPosition({ "0.78125%", "1.25%" });
	gui.add(backButton);

	borrowButton->onPress([this, bookId]
		{
			auto successWindow = tgui::ChildWindow::create();
			successWindow->setSize({ "31.25%", "37.5%" });
			successWindow->setOrigin(0.5f, 0.5f);
			successWindow->setPosition({ "50%", "50%" });

			auto successWindowText = tgui::Label::create();
			auto okButton = tgui::Button::create("OK");
			successWindow->add(successWindowText);
			successWindow->add(okButton);

			successWindowText->setAutoSize(true);
			successWindowText->setOrigin(0.5f, 0.5f);
			successWindowText->setPosition({ "50%", "50%" });

			okButton->setSize({ "15.25%", "8.66%" });
			okButton->setPosition({ "35%", "85%" });
			okButton->onPress([this, successWindow] {gui.remove(successWindow); });

			this->request.clear();
			this->makePacket(Requests::BorrowBook);
			this->sendPacket(this->request);
			if (this->waitForConfirmation())
			{
				this->packet.clear();
				packet << userId << bookId;
				this->sendPacket(this->packet);
				this->receivePackets(this->packet);
				std::string response;
				packet >> response;
				std::cout << response;
				if (response == "BORROW | OK")
				{
					successWindowText->setText("Book borrowed successfully, you can now view it on your profile page.");
					gui.add(successWindow);
				}
				else
				{
					successWindowText->setText("Book could not be borrowed. (Have you already borrowed 5 books?)");
					gui.add(successWindow);
				}
			}
		}); 
	backButton->onPress([this] {runMainMenuPage(); });
}

bool Library::runRegisterPage()
{
	try
	{
		loadRegisterPageWidgets();
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

bool Library::runLoginPage()
{
	try
	{
		loadLoginPageWidgets();
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

bool Library::runMainMenuPage()
{
	try
	{
		loadMainMenuPageWidgets();
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

bool Library::runUserProfilePage()
{
	try
	{
		loadUserProfilePageWidgets();
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

bool Library::runPreviewBookPage(int bookId)
{
	try
	{
		loadPreviewBookPageWidgets(bookId);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

bool Library::runReadBookPage(int bookId)
{
	try
	{
		loadReadBookPageWidgets(bookId);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}


//server functions

void Library::connect(std::string address, unsigned short port)
{
	if (this->socket.connect(address, port) != sf::Socket::Done)
	{
		std::cout << "Could not connect to the server" << std::endl;
	}
	else
	{
		this->isConnected = true;
		std::cout << "Connected";
		
	}
}

void Library::receivePackets(sf::Packet& packet)
{
	
	packet.clear();
	socket.receive(packet);

	
}

void Library::sendPacket(sf::Packet& packet)
{
	if (packet.getDataSize() > 0 && this->socket.send(packet) != sf::Socket::Done)
	{
		std::cout << "Could not send packet";
	}
}

void Library::makePacket(Requests request)
{
	this->request << static_cast<int>(request);
}


void Library::waitForRequest()
{
	
		while (true)
		{
			
			this->socket.receive(this->packet);
			if (packet.getDataSize() > 0)
			{
				//to do
				//receive the answer from server and execute it
				//writing to console temporary
				std::string receivedString;
				packet >> receivedString;
				std::cout << receivedString;
				break;
			}
		}

	
}

bool Library::waitForConfirmation()
{
	this->request.clear();
	while (true)
	{
		this->socket.receive(this->request);
		if (request.getDataSize() > 0)
		{
			bool confirmation;
			request >> confirmation;
			return confirmation;
		}
		
	}
}


void Library::run()
{
	//hardcoded for testing
	connect("localHost", 2000);

	runLoginPage();
	while (this->window->isOpen())
	{
		sf::Event event;
		while (this->window->pollEvent(event))
		{
			gui.handleEvent(event);

			if (event.type == sf::Event::Closed)
				window->close();
		}

		window->clear();
		gui.draw();
		window->display();
	}
}