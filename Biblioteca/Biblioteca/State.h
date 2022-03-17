#pragma once
#include<vector>
#include<stack>
#include<map>
#include<SFML/Graphics.hpp>
#include<iostream>
#include<TGUI/TGUI.hpp>

class State
{
protected:
	std::stack<State*>* states;
	sf::RenderWindow* window;
	tgui::GuiSFML gui;
	std::vector<sf::Texture*> textures;
	bool quit;

	sf::Vector2i mousePosScreen;
	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosView;

public:
	State(sf::RenderWindow* window, std::stack<State*>* states);
	virtual ~State();

	bool getQuit() const;
	virtual void checkForQuit();
	virtual void endState() = 0;

	virtual void updateMousePositions();
	virtual void updateKeybinds(const float& deltaTime) = 0;
	virtual void update(const float& deltaTime)=0;
	virtual void render(sf::RenderTarget* target=nullptr)=0;
};