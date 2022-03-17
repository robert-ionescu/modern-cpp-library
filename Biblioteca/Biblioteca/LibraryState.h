#pragma once
#include"State.h"


class LibraryState:
	public State
{
public:
	LibraryState(sf::RenderWindow* window, std::stack<State*>* states);
	virtual ~LibraryState();

	//Funtions
	void endState();

	void updateKeybinds(const float& deltaTime);
	void update(const float& deltaTime);
	void render(sf::RenderTarget* target=nullptr);

};
