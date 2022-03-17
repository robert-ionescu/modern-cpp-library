#pragma once

#include<SFML/Graphics.hpp>
enum class textBoxStates
{
    StateDefault,
    StateFocused
};

class TextBox
{
private:
    
    sf::Text    text;
    int borderSize;
    float padding;

    textBoxStates state;
    sf::RectangleShape shape;

    mutable sf::RectangleShape cursor;
    mutable sf::Clock  cursorTimer;
    size_t             cursorPos;
    size_t             maxLength;

public:
    TextBox(float x, float y, float width, float height, sf::Font* font);

    void setText(const sf::String& string);
    const sf::String& getText() const;
    void setMaxLength(size_t maxLength);

    void setCursor(size_t index);
    size_t getCursor() const;
    void draw(sf::RenderTarget& target/*, sf::RenderStates states*/);

protected:

    void onKeyPressed(const sf::Event::KeyEvent& key);
    void onMousePressed(float x, float y);
    void onTextEntered(sf::Uint32 unicode);
    void onStateChanged(textBoxStates state);
};
