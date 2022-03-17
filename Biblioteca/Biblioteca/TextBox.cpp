#include "TextBox.h"
#include <SFML/OpenGL.hpp>

const float BLINK_PERIOD = 1.f;

TextBox::TextBox(float x, float y, float width, float height, sf::Font* font) :
    cursorPos(0),
    maxLength(256)
{
    borderSize = 1.f;
    padding = 1.f;

    this->shape.setSize(sf::Vector2f(width, height));

    int offset = borderSize + padding;
    text.setFont(*font);
    text.setPosition(offset, offset);
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(12);

    cursor.setPosition(offset, offset);
    int lineSpacing = font->getLineSpacing(text.getCharacterSize());
    cursor.setSize(sf::Vector2f(1.f, lineSpacing));
    cursor.setFillColor(sf::Color::Black);
    setCursor(0);
}


void TextBox::setText(const sf::String& string)
{
    if (string.getSize() > maxLength)
    {
        text.setString(string.substring(0, maxLength));
    }
    else
    {
        text.setString(string);
    }
    setCursor(getText().getSize());
}


const sf::String& TextBox::getText() const
{
    return text.getString();
}


void TextBox::setMaxLength(size_t maxLength)
{
    maxLength = maxLength;
    // Trim current text if needed
    if (text.getString().getSize() > maxLength)
    {
        text.setString(text.getString().substring(0, maxLength));
        setCursor(maxLength);
    }
}


void TextBox::setCursor(size_t index)
{
    if (index <= text.getString().getSize())
    {
        cursorPos = index;

        float PADDING = borderSize + padding;
        cursor.setPosition(text.findCharacterPos(index).x, PADDING);
        cursorTimer.restart();

        if (cursor.getPosition().x > this->shape.getSize().x - PADDING)
        {
            float diff = cursor.getPosition().x - this->shape.getSize().x + PADDING;
            text.move(-diff, 0);
            cursor.move(-diff, 0);
        }
        else if (cursor.getPosition().x < PADDING)
        {
            float diff = PADDING - cursor.getPosition().x;
            text.move(diff, 0);
            cursor.move(diff, 0);
        }

        float text_width = text.getLocalBounds().width;
        if (text.getPosition().x < padding
            && text.getPosition().x + text_width < this->shape.getSize().x - padding)
        {
            float diff = (this->shape.getSize().x - PADDING) - (text.getPosition().x + text_width);
            text.move(diff, 0);
            cursor.move(diff, 0);
            if (text_width < (this->shape.getSize().x - PADDING * 2))
            {
                diff = PADDING - text.getPosition().x;
                text.move(diff, 0);
                cursor.move(diff, 0);
            }
        }
    }
}


size_t TextBox::getCursor() const
{
    return cursorPos;
}


void TextBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    case sf::Keyboard::Left:
        setCursor(cursorPos - 1);
        break;

    case sf::Keyboard::Right:
        setCursor(cursorPos + 1);
        break;

    case sf::Keyboard::BackSpace:
        if (cursorPos > 0)
        {
            sf::String string = text.getString();
            string.erase(cursorPos - 1);
            text.setString(string);

            setCursor(cursorPos - 1);
        }
        break;

    case sf::Keyboard::Delete:
        if (cursorPos < text.getString().getSize())
        {
            sf::String string = text.getString();
            string.erase(cursorPos);
            text.setString(string);

            setCursor(cursorPos);
        }
        break;

    case sf::Keyboard::Home:
        setCursor(0);
        break;

    case sf::Keyboard::End:
        setCursor(text.getString().getSize());
        break;

    /*case sf::Keyboard::Return:
        break;*/
    
    default:
        break;
    }
}


void TextBox::onMousePressed(float x, float)
{
    for (int i = text.getString().getSize(); i >= 0; --i)
    {
        sf::Vector2f glyph_pos = text.findCharacterPos(i);
        if (glyph_pos.x <= x)
        {
            setCursor(i);
            break;
        }
    }
}


void TextBox::onTextEntered(sf::Uint32 unicode)
{
    if (unicode > 30 && (unicode < 127 || unicode > 159))
    {
        sf::String string = text.getString();
        if (string.getSize() < maxLength)
        {
            // Insert character in string at cursor position
            string.insert(cursorPos, unicode);
            text.setString(string);
            setCursor(cursorPos + 1);
        }
    }
}


void TextBox::onStateChanged(textBoxStates state)
{
    this->state = state;
}

void TextBox::draw(sf::RenderTarget& target/*, sf::RenderStates states*/) 
{   
   /* sf::Transform transform = sf::Transform(
        1, 0, (int)this->shape.getPosition().x,
        0, 1, (int)(int)this->shape.getPosition().y,
        0, 0, 1);
    states.transform *= transform;
    target.draw(this->shape, states);*/

    // Crop the text with GL Scissor
    //glEnable(GL_SCISSOR_TEST);
    //sf::Vector2f pos = this->shape.getTransform().transformPoint(this->shape.getPosition());
    //glScissor(pos.x + borderSize, target.getSize().y - (pos.y + this->shape.getSize().y), this->shape.getSize().x, this->shape.getSize().y);
    //target.draw(text);

    //glDisable(GL_SCISSOR_TEST);

    // Show cursor if focused
    if (this->state == textBoxStates::StateFocused)
    {
        // Make it blink
        float timer = cursorTimer.getElapsedTime().asSeconds();
        if (timer >= BLINK_PERIOD)
            cursorTimer.restart();

        sf::Color color = sf::Color::Black;
        color.a = 255 - (255 * timer / BLINK_PERIOD);
        cursor.setFillColor(color);

        target.draw(cursor);
    }
}
