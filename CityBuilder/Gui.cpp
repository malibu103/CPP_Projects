#include "Gui.h"

sf::Vector2f Gui::getSize()
{
	return sf::Vector2f(this->dimensions.x, this->dimensions.y * this->entries.size()); // get size of TOTAL gui element (all entries combined)
}

int Gui::getEntry(const sf::Vector2f mousePos)
{
	// if there are no entries then outside the menu
	if (entries.size() == 0) return -1;
	if (!this->visible) return -1;

	for (int i = 0; i < this->entries.size(); ++i)  // loop entries
	{
		// translate point to use the entry's local coordinates
		sf::Vector2f point = mousePos;
		point += this->entries[i].shape.getOrigin(); // adds to the mouse position vector by the origin of our menu entry
		point -= this->entries[i].shape.getPosition(); // subtract the position of the menu entry on the gui view

		if (point.x < 0 || point.x > this->entries[i].shape.getScale().x*this->dimensions.x) continue; // we are outside of the x dimension
		if (point.y < 0 || point.y > this->entries[i].shape.getScale().y*this->dimensions.y) continue; // we are outside of the y dimension
		return i; // we arent outside...which means we are inside.  return this entry index as it matches!
	}

	return -1;
}

void Gui::setEntryText(int entry, std::string text)
{
	if (entry >= entries.size() || entry < 0) return; // avoid exception if out of bounds

	entries[entry].text.setString(text); // set the entry text

	return;
}

void Gui::setDimensions(sf::Vector2f dimensions)
{
	this->dimensions = dimensions; // resetting object dimensions to supplied values

	for (auto& entry : this->entries) // loop through our entries
	{
		entry.shape.setSize(dimensions); // set our size
		entry.text.setCharacterSize(dimensions.y - style.borderSize - padding); // same as in constructor, set new character size based on size of our entry
	}

	return;
}

void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!visible) return; // dont draw...we are invis!

	// draw each entry of the menu
	for (auto entry : this->entries)
	{
		// draw the entry
		target.draw(entry.shape);
		target.draw(entry.text);
	}

	return;
}

void Gui::show()
{
	sf::Vector2f offset(0.0f, 0.0f);

	this->visible = true; // make us visible, so we will draw

	// draw each entry of the menu
	for (auto& entry : this->entries)
	{
		// set the origin of the entry
		sf::Vector2f origin = this->getOrigin();
		origin -= offset;
		entry.shape.setOrigin(origin);
		entry.text.setOrigin(origin);

		// compute the position of the entry
		entry.shape.setPosition(this->getPosition());
		entry.text.setPosition(this->getPosition());

		// which axis to increment on, based on our horizontal flag
		if (this->horizontal) offset.x += this->dimensions.x;
		else offset.y += this->dimensions.y;
	}

	return;
}

void Gui::hide()
{
	this->visible = false; // simple...make us invis
	return;
}

// highlights an entry of the menu
void Gui::highlight(const int entry)
{
	for (int i = 0; i < entries.size(); ++i) // loop our entries
	{
		if (i == entry) // we are at our entry...highlight it!
		{
			entries[i].shape.setFillColor(style.bodyHighlightCol);
			entries[i].shape.setOutlineColor(style.borderHighlightCol);
			entries[i].text.setColor(style.textHighlightCol);
		}
		else // this is not us, lets unhighlight it...eh?
		{
			entries[i].shape.setFillColor(style.bodyCol);
			entries[i].shape.setOutlineColor(style.borderCol);
			entries[i].text.setColor(style.textCol);
		}
	}

	return;
}

// return the messge bound to the entry
std::string Gui::activate(const int entry)
{
	if (entry == -1) return "null";
	return entries[entry].message;
}

// return message bound to entry based on mouse position
std::string Gui::activate(sf::Vector2f mousePos)
{
	int entry = this->getEntry(mousePos);
	return this->activate(entry);
}