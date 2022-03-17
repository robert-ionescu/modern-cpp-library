#pragma once
#include"LibraryState.h"
#include"Button.h"
#include"TextBox.h"

class LoginState:public State
{
private:
	sf::RectangleShape background;
	sf::RectangleShape logInBox;
	sf::Font font;
	
	tgui::GuiSFML gui;
	TextBox* textBox;

	void initFonts();

public:
	LoginState(sf::RenderWindow* window, std::stack<State*>* states);
	virtual ~LoginState();

	//Funtions
	void endState();

	void updateKeybinds(const float& deltaTime);
	void update(const float& deltaTime);
	void render(sf::RenderTarget* target = nullptr);

};
