#include"LoginState.h"
#include"TGUI/TGUI.hpp"

void LoginState::initFonts()
{
	if (!this->font.loadFromFile("Dosis-Light.ttf"))
	{
		throw("Could not load font");
	}
}

LoginState::LoginState(sf::RenderWindow* window, std::stack<State*>* states)
	:State(window, states)
{
	this->initFonts();

	
	this->background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	this->background.setFillColor(sf::Color(225, 220, 197));
	
	this->logInBox.setSize(sf::Vector2f(window->getSize().x / 2, window->getSize().y / 2));
	this->logInBox.setFillColor(sf::Color::White);
	this->logInBox.setOrigin(sf::Vector2f(window->getSize().x / 4, window->getSize().y / 4));
	this->logInBox.setPosition(sf::Vector2f(window->getSize().x / 2, window->getSize().y / 2));
	this->logInBox.setOutlineThickness(1.f);
	this->logInBox.setOutlineColor(sf::Color::Black);

	this->textBox = new TextBox(100.f, 100.f, 10.f, 10.f, &this->font);

}

LoginState::~LoginState()
{
}

void LoginState::endState()
{
	std::cout << "ending loginState";
}

void LoginState::updateKeybinds(const float& deltaTime)
{
	this->checkForQuit();
}


void LoginState::update(const float& deltaTime)
{

	this->updateKeybinds(deltaTime);
	this->updateMousePositions();
	//just a test
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		std::cout << "A";
	}

}


void login(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password)
{
	std::cout << "Username: " << username->getText() << std::endl;
	std::cout << "Password: " << password->getText() << std::endl;
}

void updateTextSize(tgui::GuiBase& gui)
{
	// Update the text size of all widgets in the gui, based on the current window height
	const float windowHeight = gui.getView().getRect().height;
	gui.setTextSize(static_cast<unsigned int>(0.07f * windowHeight)); // 7% of height
}

void loadWidgets(tgui::GuiBase& gui)
{
	// Specify an initial text size instead of using the default value
	updateTextSize(gui);

	// We want the text size to be updated when the window is resized
	gui.onViewChange([&gui] { updateTextSize(gui); });

	// Create the background image
	// The picture is of type tgui::Picture::Ptr which is actually just a typedef for std::shared_widget<tgui::Picture>
	// The picture will fit the entire window and will scale with it
	auto picture = tgui::Picture::create("../xubuntu_bg_aluminium.jpg");
	picture->setSize({ "100%", "100%" });
	gui.add(picture);

	// Create the username edit box
	// Similar to the picture, we set a relative position and size
	// In case it isn't obvious, the default text is the text that is displayed when the edit box is empty
	auto editBoxUsername = tgui::EditBox::create();
	editBoxUsername->setSize({ "66.67%", "12.5%" });
	editBoxUsername->setPosition({ "16.67%", "16.67%" });
	editBoxUsername->setDefaultText("Username");
	gui.add(editBoxUsername);

	// Create the password edit box
	// We copy the previous edit box here and keep the same size
	auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
	editBoxPassword->setPosition({ "16.67%", "41.6%" });
	editBoxPassword->setPasswordCharacter('*');
	editBoxPassword->setDefaultText("Password");
	gui.add(editBoxPassword);

	// Create the login button
	auto button = tgui::Button::create("Login");
	button->setSize({ "50%", "16.67%" });
	button->setPosition({ "25%", "70%" });
	gui.add(button);

	// Call the login function when the button is pressed and pass the edit boxes that we created as parameters
	// The "&" in front of "login" can be removed on newer compilers, but is kept here for compatibility with GCC < 8.
	button->onPress(&login, editBoxUsername, editBoxPassword);
}

bool runExample(tgui::GuiBase& gui)
{
	try
	{
		loadWidgets(gui);
		return true;
	}
	catch (const tgui::Exception& e)
	{
		std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
		return false;
	}
}

void LoginState::render(sf::RenderTarget* target)
{
	if (!target)
	{
		target = this->window;
	}
	//target->draw(this->background);
	//target->draw(this->logInBox);

	//this->renderButtons(target);
	tgui::GuiSFML gui{ *target };
	runExample(gui);
	gui.draw();
}