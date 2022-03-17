#include"LibraryState.h"

LibraryState::LibraryState(sf::RenderWindow* window, std::stack<State*>* states)
	:State(window, states)
{

}

LibraryState::~LibraryState()
{
}

void LibraryState::endState()
{
	std::cout << "ending libraryState";
}

void LibraryState::updateKeybinds(const float& deltaTime)
{
	this->checkForQuit();
}

void LibraryState::update(const float& deltaTime)
{

	this->updateKeybinds(deltaTime);
	this->updateMousePositions();
	//just a test
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		std::cout << "A";
	}
}

void LibraryState::render(sf::RenderTarget* target)
{
	if (!target)
	{
		target = this->window;
	}
	
}
