#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>
using namespace sf;
int fps = 60;
double G = 1;
double T = 1;
double zoom = 10;
double viewZoom = 1;
double zoomChange = 1.05;
float resolutionWidth = 800.0f,	resolutionHeight = 600.0f;


template <typename T>
std::string to_string(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}



struct vect{
	double x;
	double y;
};

struct planet{
	std::string name;
	double mass;
	vect speed;
	vect pos;
	CircleShape circle;
};

std::vector <planet> list(0);


void resizeMap(const RenderWindow& window, View& view) {
	view.setSize(window.getSize().x * viewZoom, window.getSize().y * viewZoom);
}

void resizeView(const RenderWindow& window, View& view) {
	view.setSize(window.getSize().x, window.getSize().y);
	view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
}

void addPlanet(std::string name, double mass,double x,double y,double vx,double vy,double radius, Color color) {
	planet newPlanet;
	newPlanet.name = name;
	newPlanet.mass = mass;
	newPlanet.speed.x = vx;
	newPlanet.speed.y = vy;
	newPlanet.pos.x = x;
	newPlanet.pos.y = y;
	newPlanet.circle.setRadius(radius*zoom);
	newPlanet.circle.setFillColor(color);
	newPlanet.circle.setPosition((x*zoom)-radius*zoom,(y*zoom)-radius*zoom);
	
	list.push_back(newPlanet);
}

Color randomColorId() {
	int minNum = 0;
	int maxNum = 6;
	int rn = ( std::rand() % ( maxNum - minNum + 1 ) );
	switch (rn) {
		case 1:
			return Color::Red;
		case 2:
			return Color::Magenta;
		case 3:
			return Color::Blue;
		case 4:
			return Color::Cyan;
		case 5:
			return Color::Green;
		case 6:
			return Color::Yellow;
		//case default:
		//	return Color::White;
	}
}

Color randomColor() {
	int minNum = 0;
	int maxNum = 255;
	int red = ( std::rand() % ( maxNum - minNum + 1 ) );
	int blue = ( std::rand() % ( maxNum - minNum + 1 ) );
	int green = ( std::rand() % ( maxNum - minNum + 1 ) );
	Color col(red,blue,green,255);
	return col;
}

void defaultPlanets() {
	//addPlanet 	("name",	MASS,	X,		Y,		VX,		VY,		RAD,	COLOR);
	addPlanet		("Earth",	5,		32,		25,		0,		2,   	1,		Color::Blue);
	addPlanet		("Sun",		20,		25,		25,		0,		0,		1.5,		Color::Yellow);
	
	list[0].circle.setOutlineThickness(3);
	list[0].circle.setOutlineColor(Color::Green);
	/*Texture earth_texture;
		earth_texture.loadFromFile("data/imgs/minimal-earth1.png");
	list[0].circle.setTexture(earth_texture,true);*/
	
	list[1].circle.setOutlineThickness(6);
	list[1].circle.setOutlineColor(Color::Red);
}


bool MouseOverSprite(RenderWindow& window, Sprite& sprite) {
	if(	sf::Mouse::getPosition(window).x > sprite.getGlobalBounds().left
	and	sf::Mouse::getPosition(window).x < sprite.getGlobalBounds().left + sprite.getGlobalBounds().width
	and	sf::Mouse::getPosition(window).y > sprite.getGlobalBounds().top
	and	sf::Mouse::getPosition(window).y < (sprite.getGlobalBounds().top + sprite.getGlobalBounds().height))
	{
		return true;
	}
	else return false;
}
		
		

//GENIS FUNCTIONS
double delta(planet ob1, planet ob2) {
	double disx = ob1.pos.x - ob2.pos.x, disy = ob1.pos.y - ob2.pos.y;
	return sqrt(disx*disx + disy * disy);
}

void newPos(planet& ob) {
	ob.pos.x += T * ob.speed.x;
	ob.pos.y += T * ob.speed.y;
	ob.circle.setPosition((ob.pos.x*zoom)-ob.circle.getRadius(),(ob.pos.y*zoom)-ob.circle.getRadius());
}

vect unit(planet ob1, planet ob2) {
	vect v;
	double vx = ob1.pos.x - ob2.pos.x;
	double vy = ob1.pos.y - ob2.pos.y;
	v.x = vx / delta(ob1, ob2);
	v.y = vy / delta(ob1, ob2);
	return v;
}

bool nvel(planet& ob, std::vector<planet> list, int nob) {
	int s = list.size();
	for (int i = 0; i < s; ++i) {
		if (i != nob) {
			double r = delta(ob, list[i]);
			if (r==0) {
				return true;
				}
			else {
				double f = -(G*ob.mass*list[i].mass) / (r*r);
				double a = f / ob.mass;
				vect v = unit(ob, list[i]);
				ob.speed.x += T * a* v.x;
				ob.speed.y += T * a* v.y;
			}
		}
	}
	return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





int main() {
	T = T/fps;
	RenderWindow window(VideoMode(resolutionWidth, resolutionHeight),"Gravity");
	window.setFramerateLimit(fps);
	//View view(Vector2f(resolutionWidth/2,resolutionHeight/2),Vector2f(resolutionWidth,resolutionHeight));
	View view(Vector2f(resolutionWidth/2,resolutionHeight/2),Vector2f(resolutionWidth,resolutionHeight));
	View mainView(Vector2f(resolutionWidth/2,resolutionHeight/2),Vector2f(resolutionWidth,resolutionHeight));
	int menuSelect=1;
	
	
	
//FONTS
	Font product_sans;
	product_sans.loadFromFile("data/fonts/Product Sans Regular.ttf");
	Font bold_product_sans;
	bold_product_sans.loadFromFile("data/fonts/Product Sans Bold.ttf");

	
//TEXTURES
	Texture space_texture;
		space_texture.loadFromFile("data/imgs/space-dim.jpg");
	
	Texture hud_cursor_texture;
		hud_cursor_texture.loadFromFile("data/imgs/cursor.png");
		
	Texture hud_menu_texture;
		hud_menu_texture.loadFromFile("data/imgs/hud-menu.png");
	Texture hud_menu_highlight_texture;
		hud_menu_highlight_texture.loadFromFile("data/imgs/hud-menu-highlight.png");
	Texture hud_menu_selected_texture;
		hud_menu_selected_texture.loadFromFile("data/imgs/hud-menu-selected.png");
	Texture hud_option_texture;
		hud_option_texture.loadFromFile("data/imgs/hud-menu-option.png");
	Texture hud_option_newplanet_texture;
		hud_option_newplanet_texture.loadFromFile("data/imgs/hud-menu-option-newplanet.png");
		
//SPRITES
	Sprite background(space_texture);
	Sprite cursor(hud_cursor_texture);
	Sprite hud_menu(hud_menu_texture);
	Sprite hud_menu_selected(hud_menu_selected_texture);
	Sprite hud_menu_highlight(hud_menu_highlight_texture);
		Sprite hud_option1(hud_option_texture);
		Sprite hud_option2(hud_option_texture);
		Sprite hud_option3(hud_option_texture);
		Sprite hud_option4(hud_option_texture);
		Sprite hud_option5(hud_option_texture);
	Sprite hud_option_newplanet(hud_option_newplanet_texture);
	
	
//SHAPES
	

//MUSIC
	Music bubble;
	bubble.openFromFile("data/audio/En-us-bubble.ogg");
	Music blop;
	blop.openFromFile("data/audio/blop.ogg");

//TEXT
	Text index(" Press R to restart simulation\n Press I to show information\n Press U to delete last object\n Press X to delete all objects\n Press Space to pause simulation",product_sans,16);
	Text index2("    Zoom:\nVelocity:\nObjects:",product_sans,16);
	index2.setPosition(260,0);






	

	defaultPlanets();

	vect mousePos;
	vect oldMousePos;
	vect windowOrigin;
	mousePos.x = 1;
	mousePos.y = 1;
	oldMousePos.x = 1;
	oldMousePos.y = 1;
	windowOrigin.x = 0;
	windowOrigin.y = 0;
	bool pressing = false;
	bool dragging = false;
	bool showInfo = false;
	bool pause = false;
	
//MAIN BUCLE
	while (window.isOpen()) {
		mousePos.x = ((double(Mouse::getPosition(window).x) - window.getSize().x/2)*viewZoom) + (view.getCenter().x);
		mousePos.y = ((double(Mouse::getPosition(window).y) - window.getSize().y/2)*viewZoom) + (view.getCenter().y);
		windowOrigin.x = view.getCenter().x - view.getSize().x/2;
		windowOrigin.y = view.getCenter().y - view.getSize().y/2;
		
		
		
		Event event; 
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) window.close();
			if (event.type == sf::Event::Resized) {
				resizeMap(window,view);
				resizeView(window,mainView);
        	}
        	if (event.type == Event::KeyPressed) {
        		if (Keyboard::isKeyPressed(Keyboard::R)) {
        			list.clear();
        			zoom = 10;
        			defaultPlanets();
        		}
        		if (Keyboard::isKeyPressed(Keyboard::I)) {
        			if (showInfo) {
						showInfo = false;
					}
					else {
						showInfo = true;
					}
        		}
        		if (Keyboard::isKeyPressed(Keyboard::U)) {
        			if (list.size()>0) list.pop_back();
        		}
				if (Keyboard::isKeyPressed(Keyboard::X)) {
        			list.clear();
        		}
				if (Keyboard::isKeyPressed(Keyboard::Space)) {
        			if (pause) pause = false;
        			else pause = true;
        		}
        		if (Keyboard::isKeyPressed(Keyboard::Add)) {
					T*=1.5;
				}
        		if (Keyboard::isKeyPressed(Keyboard::Subtract)) {
					T/=1.5;
				}
        	}
        	
        	if (event.type == Event::MouseButtonPressed and event.key.code == Mouse::Left) {
        		if (MouseOverSprite(window,hud_option1)) menuSelect=1;
        		if (MouseOverSprite(window,hud_option2)) menuSelect=2;
        		if (MouseOverSprite(window,hud_option3)) menuSelect=3;
        		if (MouseOverSprite(window,hud_option4)) menuSelect=4;
        		if (MouseOverSprite(window,hud_option5)) menuSelect=5;
        		else if (menuSelect==1 and not dragging and not pressing and not MouseOverSprite(window,hud_menu)) {
        			oldMousePos = mousePos;
        			pressing = true;
        		}
        	}
        	if (event.type == Event::MouseButtonPressed and event.key.code == Mouse::Right) {
        		if (not pressing and not dragging and not MouseOverSprite(window,hud_menu)) {
        			oldMousePos = mousePos;
					dragging=true;
				}
        	}
        	if (event.type == Event::MouseButtonReleased and event.key.code == Mouse::Left) {
        		if (pressing) {
        			blop.play();
        			addPlanet ("Asteroid", 2, oldMousePos.x/zoom, oldMousePos.y/zoom, (mousePos.x - oldMousePos.x)/zoom, (mousePos.y - oldMousePos.y)/zoom, 1.5, randomColor());
        			pressing = false;
        		}
        	}
        	if (event.type == Event::MouseButtonReleased and event.key.code == Mouse::Right) {
				if (dragging) dragging = false;
			}
        	
        	if (event.type == Event::MouseWheelMoved) {
        		if (event.mouseWheel.delta > 0) {
					viewZoom/=zoomChange;
					resizeMap(window,view);
				}
        		else if (event.mouseWheel.delta < 0) {
					viewZoom*=zoomChange;
					resizeMap(window,view);
        		}
        	}
		}
		
		if (not pause) {
			for (int j=0; j < T; ++j) {
				for (int i = 0; i < list.size(); ++i) {
					newPos(list[i]);
				}
				for (int i = 0; i < list.size(); ++i) {
					if (nvel(list[i], list, i)) {
						system ("echo ERROR:\nTwo objects were in the exact same position&pause");
						return EXIT_FAILURE;
					}
				}
			}
		}
		
		
		
		
		
	//GRAPHICS:
		if (dragging) {
			view.setCenter((view.getCenter().x) + (oldMousePos.x) - (mousePos.x), (view.getCenter().y) + (oldMousePos.y) - (mousePos.y));
		}
	
		window.clear();
		window.setView(view);
		
		
		window.draw(background);
		
		for (int k=0; k<list.size(); ++k) {
			window.draw(list[k].circle);
			
			if (showInfo) {
				Text planetName(list[k].name, bold_product_sans, 20);
				planetName.setPosition(list[k].circle.getPosition().x, list[k].circle.getPosition().y - 65);
				window.draw(planetName);
				
				Text planetMass1("Mass:", product_sans, 16);
				planetMass1.setPosition(list[k].circle.getPosition().x, list[k].circle.getPosition().y - 45);
				window.draw(planetMass1);
				
				Text planetMass2(to_string(list[k].mass), product_sans, 16);
				planetMass2.setPosition(list[k].circle.getPosition().x+45, list[k].circle.getPosition().y - 45);
				window.draw(planetMass2);
				
				Text planetSpeed1("Speed:", product_sans, 16);
				planetSpeed1.setPosition(list[k].circle.getPosition().x, list[k].circle.getPosition().y - 30);
				window.draw(planetSpeed1);
				
				double currentspeed = sqrt(list[k].speed.x*list[k].speed.x + list[k].speed.y*list[k].speed.y);
				Text planetSpeed2(to_string(currentspeed), product_sans, 16);
				planetSpeed2.setPosition(list[k].circle.getPosition().x+55, list[k].circle.getPosition().y - 30);
				window.draw(planetSpeed2);
				
			}
		}
		
		
		if (pressing) {
			VertexArray line1(LinesStrip,2);
			line1[1].position = Vector2f(mousePos.x, mousePos.y);
			line1[0].position = Vector2f(oldMousePos.x, oldMousePos.y);
			window.draw(line1);
			if (showInfo) {
				double launchSpeedNum = sqrt((((mousePos.x - oldMousePos.x)/zoom)*((mousePos.x - oldMousePos.x)/zoom))+(((mousePos.y - oldMousePos.y)/zoom)*((mousePos.y - oldMousePos.y)/zoom)));
				Text launchSpeed(to_string(launchSpeedNum),product_sans,16);
				launchSpeed.setPosition(mousePos.x-20,mousePos.y-30);
				window.draw(launchSpeed);
			}
		}
		
		//Text zoomText(to_string(zoom),product_sans,16);
		
		
		// UI:
		window.setView(mainView);
		
		
		index.setPosition(0,0);
		index.setCharacterSize(16);
		index2.setPosition(260,0);
		index2.setCharacterSize(16);
		Text zoomText(to_string(viewZoom),product_sans,16);
		Text speedText(to_string(T*fps),product_sans,16);
		Text objectsText(to_string(list.size()),product_sans,16);
		double textLine1 = 0;
		zoomText.setPosition(330,0);
		zoomText.setCharacterSize(16);
		speedText.setPosition(330,18);
		speedText.setCharacterSize(16);
		objectsText.setPosition(330,18+18);
		objectsText.setCharacterSize(16);
		
		//HUD
		
		
		
		window.draw(index);
		window.draw(index2);
		window.draw(zoomText);
		window.draw(speedText);
		window.draw(objectsText);
		
		hud_menu.setPosition(window.getSize().x/2 - hud_menu.getGlobalBounds().width/2, window.getSize().y - hud_menu.getGlobalBounds().height - 50);
			hud_option1.setPosition(hud_menu.getPosition().x + 6, hud_menu.getPosition().y+8);
			hud_option2.setPosition(hud_menu.getPosition().x + (6 + 1*49), hud_menu.getPosition().y+8);
			hud_option3.setPosition(hud_menu.getPosition().x + (6 + 2*49), hud_menu.getPosition().y+8);
			hud_option4.setPosition(hud_menu.getPosition().x + (6 + 3*49), hud_menu.getPosition().y+8);
			hud_option5.setPosition(hud_menu.getPosition().x + (6 + 4*49), hud_menu.getPosition().y+8);
		hud_option_newplanet.setPosition(hud_option1.getPosition());
		
		bool highlight=true;
		if (menuSelect!=1 and MouseOverSprite(window,hud_option1)) hud_menu_highlight.setPosition(hud_menu.getPosition().x + (6 + 0*49), hud_menu.getPosition().y+8);
		else if (menuSelect!=2 and MouseOverSprite(window,hud_option2)) hud_menu_highlight.setPosition(hud_menu.getPosition().x + (6 + 1*49), hud_menu.getPosition().y+8);
		else if (menuSelect!=3 and MouseOverSprite(window,hud_option3)) hud_menu_highlight.setPosition(hud_menu.getPosition().x + (6 + 2*49), hud_menu.getPosition().y+8);
		else if (menuSelect!=4 and MouseOverSprite(window,hud_option4)) hud_menu_highlight.setPosition(hud_menu.getPosition().x + (6 + 3*49), hud_menu.getPosition().y+8);
		else if (menuSelect!=5 and MouseOverSprite(window,hud_option5)) hud_menu_highlight.setPosition(hud_menu.getPosition().x + (6 + 4*49), hud_menu.getPosition().y+8);
		else highlight=false;
		
		window.draw(hud_menu);
		if (menuSelect>0) hud_menu_selected.setPosition(hud_menu.getPosition().x + (6 + (menuSelect-1)*49), hud_menu.getPosition().y+8);
		if (highlight) window.draw(hud_menu_highlight);
		if (menuSelect>0) window.draw(hud_menu_selected);
		
		window.draw(hud_option_newplanet);
		
		window.display();
		}
	return EXIT_SUCCESS;
}
