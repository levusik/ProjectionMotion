#pragma once
#include <exception>
#include <string>
#include <Windows.h>
#include <SFML\Graphics.hpp>
const float		FPS					 = 60.0;
const int		WINDOWWIDTH			 = 1024;
const int		WINDOWHEIGHT		 = 768;
const int		TILESIZE			 = 32;
const int		MAPEDITOROUTLINE	 = 1;
const int		XMARGIN				 = 128;
const int		YMARGINDOWN		 	 = 64;
const int		YMARGINUP			 = 8;
const int		CIRCLERADIUS		 = 25;
const int		DISTANCEBTWSECTIONS  = 32;
const int		GUIHEIGHT			 = 180;
const int		SFMLWINDOWWIDTH		 = WINDOWWIDTH - 2 * XMARGIN;
const int		SFMLWINDOWHEIGHT	 = WINDOWHEIGHT  - YMARGINUP -2*YMARGINDOWN - GUIHEIGHT;
const double	M_PI				 = 3.14159265359;
const double	EARTHGRAVITY		 = 10.;
const bool		PRINTVALUESTOTXTFILE = true;


enum class exceptionList
{
	ERROR_DURING_CLASS_INITIALIZATION,
	ERROR_DURING_CREATING_HWND,
	ERROR_DURING_LOADING_FONT,
	ERROR_DURING_STARTING_RUNNING_ANIMATION,
	INCORRECT_INPUT,
	ERROR_DURING_OPENING_FILE
};
enum class statesOfMachine
{
	INITIALIZETHROW,
	ANIMATETHROW,
	SHOWPARAMETERSOFPOINTS,

};
enum class typesOfThrow
{
	HORIZONTALTHROW, //rzut poziomy
	VERTICALTHROW,   // rzut pionowy
	CROSSWISETHROW   // rzut ukoœny
};
class exceptionWithMsg : public std::exception
{
public:
	std::string		msg;
	LPCSTR			lMsg;
	exceptionList	Val;

	exceptionWithMsg(LPCSTR msg, exceptionList typeOfException)
	{
		this->msg  = msg;
		this->lMsg = msg;
		this->Val = typeOfException;
	}
};
class Ball : public sf::Sprite
{
public:
	// konstruktory 
	Ball(int radius , sf::Vector2f position, sf::Color circleColor)
	{
		this->circleShape.setRadius(radius);
		this->circleShape.setFillColor(circleColor);
		this->circleShape.setPosition(position);
		this->setPosition(position);
	}
	Ball(sf::Texture &textureOfSprite)
	{
		this->setTexture(textureOfSprite);
	}


	// Settery 
	void setColorOfCircle(sf::Color color)
	{
		// ustawia kolor instancji klasy
		this->circleShape.setFillColor(color);
	}
	void  moveObject(sf::Vector2f position)
	{
		// zmienia pozycjê instancji klasy na dan¹ pozycjê 
		this->setPosition(position);
		circleShape.setPosition(position);
	}
	
	
	// gettery 
	sf::CircleShape getCircle()  
	{
		// zwraca SFML'owy kszta³t kó³ka
		return this->circleShape;
	}
	sf::Vector2f getCenterOfCircle() 
	{
		// zwraca pozycjê œrodka okrêgu 
		sf::Vector2f center = this->circleShape.getPosition();
		center.x += circleShape.getRadius();
		center.y += circleShape.getRadius();
		return center;
	}



private:
	sf::CircleShape circleShape;
};
class circleWithParameters : public sf::CircleShape
{
public:
	void setParameters(sf::Vector2f position, sf::Color color, int radius) {
		this->setPosition(position.x + radius, position.y + radius); 
		this->setFillColor(color);
		this->setRadius(radius);
	}
	void create(sf::Vector2f position, double timeInSec, double posX, double posY)
	{
		this->setPosition(position.x - this->getRadius(), position.y - this->getRadius());
		this->timeInSec = timeInSec;
		this->posX = posX;
		this->posY = posY;
	};
	double getTime() const
	{
		return this->timeInSec;
	};
	sf::Vector2f getCalculatedPosition() const 
	{
		return sf::Vector2f(this->posX, this->posY);
	};

protected:
double timeInSec, posX, posY;

};
class showedCircleWithParameters : public circleWithParameters
{
public:

	// ustawienie bazowych parametrów
	void setupParameters(double V, double cosA, double sinA, double G)
	{
		this->VInMeters	   = V;
		this->cosA    = cosA;
		this->sinA	  = sinA;
		this->G_const = G;
	}
	// utworzenie kó³ka
	void create(circleWithParameters circle, std::vector<circleWithParameters>::iterator index,int intIndex, sf::Color color,int scale)
	{
		orginalCircle = circle;

		setupParamsAndPositions(circle,index, intIndex, color, scale);

		calculateSizes();

	}
	// pobieramy index punktu
	int getIntIndex()
	{
		return this->indexInt;
	}	
	// rysowanie wektorów sk³adowych
	void update(sf::RenderWindow &window)
	{
		window.draw(vectorX);
		window.draw(vectorY);
		window.draw(vectorV);
	}

	// pobranie indeksu
	std::vector<circleWithParameters>::iterator getIndex() const
	{
		return this->index;
	}
	circleWithParameters getCircle()
	{
		return orginalCircle;
	}


	sf::RectangleShape vectorX, vectorY, vectorV;
	double sinA, cosA, VInMeters, G_const, Xsize, Ysize, Vsize, angleB;

private:
	void setupParamsAndPositions(circleWithParameters circle, std::vector<circleWithParameters>::iterator index,int intIndex, sf::Color color,int scale)
	{
		this->scale = scale;
		// ustawienie parametrów
		sizeOfShorterSide = 2.f;
		this->setFillColor(color);
		this->setRadius(circle.getRadius() * 4);
		this->setPosition(circle.getPosition().x - this->getRadius(), circle.getPosition().y- this->getRadius());
		this->index		= index;
		this->posX		= circle.getCalculatedPosition().x;
		this->posY		= circle.getCalculatedPosition().y;
		this->timeInSec = circle.getTime();
		this->indexInt  = intIndex;

		// ustawienie pozycji
		vectorX.setPosition(this->getPosition().x + this->getRadius(), this->getPosition().y + this->getRadius() - sizeOfShorterSide);
		vectorY.setPosition(this->getPosition().x - sizeOfShorterSide + this->getRadius(), this->getPosition().y + this->getRadius());
		vectorV.setPosition(this->getPosition().x - sizeOfShorterSide + this->getRadius(), this->getPosition().y - sizeOfShorterSide + this->getRadius());

	}
	void calculateSizes() 
	{
		// obliczanie d³ugoœci wektorów
		Xsize = VInMeters * this->scale *cosA;
		Ysize = (VInMeters * this->scale*sinA - this->G_const*this->timeInSec);
		Vsize = sqrt(std::pow(Xsize, 2) + std::pow(Ysize, 2));
	
		// ustawienie pozycji
		vectorX.setSize(sf::Vector2f(Xsize, 2 * sizeOfShorterSide));
		vectorX.setFillColor(sf::Color::Color(0xff, 0x80, 0x0));

		vectorY.setFillColor(sf::Color::Color(0x0, 0xD0, 0x0));
		vectorY.setSize(sf::Vector2f(2 * this->sizeOfShorterSide, -Ysize));

		vectorV.setFillColor(sf::Color::Color(0xff, 0xff, 0x0));
		vectorV.setSize(sf::Vector2f(2 * this->sizeOfShorterSide, -Vsize));

		angleB = atan2(vectorY.getSize().y, vectorX.getSize().x)  *180 / M_PI;

		vectorV.setRotation(90 + angleB);
	}
	int scale;
	std::vector<circleWithParameters>::iterator index;
	int  sizeOfShorterSide = 2.f, indexInt;
	circleWithParameters orginalCircle;
};