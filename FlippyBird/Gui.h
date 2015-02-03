#pragma once
#include <SFML/Graphics.hpp>

#include <vector>
#include <utility>
#include <string>

class GuiStyle
{
public:

	sf::Color bodyCol;
	sf::Color bodyHighlightCol;
	sf::Color borderCol;
	sf::Color borderHighlightCol;
	sf::Color textCol;
	sf::Color textHighlightCol;
	sf::Font* font;

	float borderSize;

	GuiStyle(sf::Font* font, float borderSize,
		sf::Color bodyCol, sf::Color borderCol, sf::Color textCol,
		sf::Color bodyHighlightCol, sf::Color borderHighlightCol, sf::Color textHighlightCol)
	{
		this->bodyCol = bodyCol;
		this->borderCol = borderCol;
		this->textCol = textCol;
		this->bodyHighlightCol = bodyHighlightCol;
		this->borderHighlightCol = borderHighlightCol;
		this->textHighlightCol = textHighlightCol;
		this->font = font;
		this->borderSize = borderSize;

	}

	GuiStyle() { }
};

class GuiEntry
{
public:
	// handle appearance of the entry
	sf::RectangleShape shape;

	// string returned when the entry is activated
	std::string message;

	// tex tdisplayed on the entry
	sf::Text text;

	GuiEntry(const std::string& message, sf::RectangleShape shape, sf::Text text)
	{
		this->message = message;
		this->shape = shape;
		this->text = text;
	}

	GuiEntry() { }
};

class Gui : public sf::Transformable, public sf::Drawable
{
private:

	// if true the menu entries will by horizontally, not vertically, adjacent
	bool horizontal;

	GuiStyle style;

	sf::Vector2f dimensions;

	int padding;

public:

	std::vector<GuiEntry> entries;

	bool visible;

	// ctor
	Gui(sf::Vector2f dimensions, int padding, bool horizontal, GuiStyle& style, std::vector<std::pair<std::string, std::string>> entries)
	{
		visible = false;
		this->horizontal = horizontal;
		this->style = style;
		this->dimensions = dimensions;
		this->padding = padding;

		// construct the background shape - Apply our styles supplied!
		sf::RectangleShape shape;
		shape.setSize(dimensions);
		shape.setFillColor(style.bodyCol);
		shape.setOutlineThickness(-style.borderSize);
		shape.setOutlineColor(style.borderCol);

		// construct each gui entry (option)
		for (auto entry : entries)
		{
			// construct the text -- applying our styles
			sf::Text text;
			text.setString(entry.first);
			text.setFont(*style.font);
			text.setColor(style.textCol);
			text.setCharacterSize(dimensions.y - style.borderSize - padding);

			this->entries.push_back(GuiEntry(entry.second, shape, text)); // creating a gui entry with the message, shape, and text...adding it to our entries vector
		}
	}

	sf::Vector2f getSize();

	// return the entry that the mouse is hovering over.  Return -1 if the mouse is outside of the Gui
	int getEntry(const sf::Vector2f mousePos);

	// change the text of an entry
	void setEntryText(int entry, std::string text);

	// change the entry dimensions
	void setDimensions(sf::Vector2f dimensions);

	// draw the menu
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void show();

	void hide();

	// highlights an entry of the menu
	void highlight(const int entry);

	// return the message bound to the entry
	std::string activate(const int entry);
	std::string activate(const sf::Vector2f mousePos);
};