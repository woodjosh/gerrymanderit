#include <cmath>
#include <cstdlib>
#include <ctime>
// delete the above when the thing is ready
#include <iostream>
#include <sstream>
#include <string>
#include "SFML/Graphics.hpp"
#include <vector>

// here are our defined headers:
#include "tract.hpp"
#include "gen_random_map.hpp"
#include "neighbor.hpp"

using namespace std;
using namespace sf;

//===========================================================================================================

class SButton
{
public:
    int x = 0, y = 0;
    bool clickState = false;

    sf::RectangleShape shape;
    sf::Color color;
    sf::Text title;

    bool isMouseOver(RenderWindow&);
    bool isClickDown(RenderWindow&);

    SButton();
    SButton(int,int,int,int,int,int,int,string);
};

SButton::SButton()
{
    x = 0;
    y = 0;
    color = sf::Color(0,0,0);

    shape.setPosition(0,0);
    shape.setFillColor(color);
    shape.setSize(Vector2f(0,0));

    Font font;
    font.loadFromFile("arial.ttf");

    title.setCharacterSize(12);
    title.setString(" ");
    title.setPosition(0,0);

    if (color.r+color.b+color.g < 384) title.setColor(Color(255,255,255));
}

SButton::SButton(int ix, int iy, int l, int w, int r, int g, int b, string t)
{
    x = ix;
    y = iy;
    color = sf::Color(r,g,b);

    shape.setPosition(ix,iy);
    shape.setFillColor(color);
    shape.setSize(Vector2f(l,w));

    title.setCharacterSize(18);
    title.setString(t);
    title.setPosition(ix+l/2-0.6*t.size()*title.getCharacterSize()/2, iy+w/2-title.getCharacterSize()/2);

    if (color.r+color.b+color.g > 384) title.setColor(Color(0,0,0));
}

bool SButton::isMouseOver(RenderWindow &window)
{
    Vector2i m = Mouse::getPosition(window);
    Vector2f s = shape.getSize();
    if (m.x > x && m.x < x+s.x && m.y > y && m.y < y+s.y)
    {
        return true;
    }
    return false;
}

bool SButton::isClickDown(RenderWindow &window)
{
    if (Mouse::isButtonPressed(Mouse::Left) && isMouseOver(window))
    {
        return true;
    }
    return false;
}

//===========================================================================================================

class testObject
{
public:
    CircleShape shape;
    double x,y;
    double radius;

    bool isClickDown(RenderWindow&);
};

bool testObject::isClickDown(RenderWindow &window)
{
    if (Mouse::isButtonPressed(Mouse::Left))
    {
        Vector2i m = Mouse::getPosition(window);
        Vector2f p = shape.getPosition();
        if (sqrt((m.x-p.x)*(m.x-p.x)+(m.y-p.y)*(m.y-p.y)) < shape.getRadius())
            return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

int main()
{
    srand(time(NULL));
    
    // C++ variables
    double frameTime;
    double o_x = 0.0;
    double o_y = 0.0;
    double rsc = 1.0;
    int wx = 1080, wy = 720;
    double ax, ay;

    bool logUpdate = false, buttonHeld = false, GUIButton = true, voterColors = false;
    int bId = -1;
    string logNew = "";
    string logList[4] = {"EXIT",
                         "RESET pressed.",
                         "OPEN pressed.",
                         "Log Cleared."};

    stringstream temp;

    vector<Tract> tracts = getTractData();
    vector<string> tractid(3500); // in main()
    vector<string> neighborId(100); //in main()
    vector < vector<string> > neighbors(3500, neighborId);// in main()
    inputArea(tracts);
    readneighbor(tractid, neighbors);
    inputNeighbor(tracts, neighbors, tractid);
    gen_random_map(tracts, 18, 0.01, pop_diff_m);

    vector<testObject> testPoints(tracts.size());
    Color districtColor[18];
    for (int i = 0; i < 18; i++)
    {
        districtColor[i] = Color(rand()%192+64,rand()%192+64,rand()%192+64);
    }
    for (int i = 0; i < testPoints.size(); i++)
    {
        testPoints[i].x = (tracts.at(i).intPtLon+91)*300;
        testPoints[i].y = (-tracts.at(i).intPtLat+40)*300;
        testPoints[i].radius = 1+sqrt(0.8*(double)tracts.at(i).area);
        //cout << "Area: " << tracts[i].area << endl;
        testPoints[i].shape.setFillColor(districtColor[tracts[i].district]);
    }

    // SFML variables
    Clock fpsTimer;
    Clock toggler;
    RenderWindow window(VideoMode(wx,wy),"GerryGui Test Suite");

    // Test: DEBUG ONLY. This may be the GUI.
    vector<SButton> test(4);

    test[0] = SButton(20,600,200,100,255,0,0,"EXIT");
    test[1] = SButton(640,600,200,100,0,178,0,"RESET");
    test[2] = SButton(860,600,200,100,0,0,255,"OPEN");
    test[3] = SButton(520,600,100,100,170,170,255,"CLEAR");

    Font font;
    font.loadFromFile("arial.ttf");
    for (int i = 0; i < test.size(); i++)
    {
        test[i].title.setFont(font);
    }

    vector<Text> logs(5,Text("",font,16));
    for (int i = 0; i < logs.size(); i++)
    {
        logs[i].setPosition(240,600+20*i);
        logs[i].setFont(font);
    }

    RectangleShape gray;
    gray.setFillColor(Color(128,128,128));
    gray.setPosition(0.f,580.f);
    gray.setSize(Vector2f(1080.f,140.f));
    // END GUI STUFF.

    // Testing render loop
    while (window.isOpen())
    {
        // check for keypresses and close buttons
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
        }

        // Handle fps updates & update title
        temp.str(string());
        frameTime = fpsTimer.restart().asSeconds();
        temp << 1.d/frameTime;

        window.setTitle(String("GerryGui Test Suite | ")+String(temp.str())+String("fps"));

        if (Keyboard::isKeyPressed(Keyboard::W)) // camera shifts north
            o_y-=400*frameTime/rsc;
        if (Keyboard::isKeyPressed(Keyboard::S)) // camera shifts south
            o_y+=400*frameTime/rsc;
        if (Keyboard::isKeyPressed(Keyboard::A)) // camera shifts west
            o_x-=400*frameTime/rsc;
        if (Keyboard::isKeyPressed(Keyboard::D)) // camera shifts east
            o_x+=400*frameTime/rsc;
        if (Keyboard::isKeyPressed(Keyboard::Up)) // camera zooms out
            rsc/=1+1*frameTime;
        if (Keyboard::isKeyPressed(Keyboard::Down)) // camera zooms in
            rsc*=1+1*frameTime;
        if (Keyboard::isKeyPressed(Keyboard::V)&&toggler.restart().asSeconds()>0.4) // toggle the graphics thing
        {
            voterColors = !voterColors;
            if (voterColors)
            {
                for (int i = 0; i < testPoints.size(); i++)
                {
                    double t = tracts[i].Vgop + tracts[i].Vdem;
                    double r = tracts[i].Vgop/t;
                    double d = tracts[i].Vdem/t;
                    testPoints[i].shape.setFillColor(Color(min(510.0*r*r,255.0),0,min(510.0*d*d,255.0)));
                }
            }
            else
            {
                for (int i = 0; i < testPoints.size(); i++)
                {
                    testPoints[i].shape.setFillColor(districtColor[tracts[i].district]);
                }
            }
        }

        // Handle updates to the district map, score, stuff like that
        // maybe use thor::ConcaveShape? that could work

        // Handle updates to the logs
        for (int i = 0; i < test.size(); i++)
        {
            if (test[i].isClickDown(window))
            {
                bId = i;
                buttonHeld = true;
                GUIButton = true;
                if (i == 3)
                {
                    logs[0].setString("");
                    logs[1].setString("");
                    logs[2].setString("");
                    logs[3].setString("");
                    logs[4].setString("");
                }
            }
        }

        // Handle the drawing stuff!!
        window.clear();

        // window.draw();
        for (int i = 0; i < testPoints.size(); i++)
        {
            testPoints[i].shape.setPosition(wx/2+(testPoints[i].x-o_x)*rsc,wy/2+(testPoints[i].y-o_y)*rsc);
            testPoints[i].shape.setOrigin(testPoints[i].radius*rsc+1,testPoints[i].radius*rsc+1);
            testPoints[i].shape.setRadius(testPoints[i].radius*rsc+1);
            ax = testPoints[i].shape.getPosition().x;
            ay = testPoints[i].shape.getPosition().y;
            if (ax > 0 && ax < wx && ay > 0 && ay < wy-140)
            {
                window.draw(testPoints[i].shape);
                if (testPoints[i].isClickDown(window))
                {
                    bId = i;
                    buttonHeld = true;
                    GUIButton = false;
                }
            }
        }

        //=================================================================

        window.draw(gray);
        for (int i = 0; i < test.size(); i++)
        {
            if (test[i].isClickDown(window)) test[i].shape.setFillColor(Color(test[i].color.r/2,test[i].color.g/2,test[i].color.b/2));
            else if (test[i].isMouseOver(window)) test[i].shape.setFillColor(Color((test[i].color.r+255)/2,(test[i].color.g+255)/2,(test[i].color.b+255)/2));
            else test[i].shape.setFillColor(test[i].color);

            window.draw(test[i].shape);
            window.draw(test[i].title);
        }

        //============================================================================

        if (buttonHeld && GUIButton && !test[bId].isClickDown(window))
        {
            logUpdate = true;
            logNew = logList[bId];
            buttonHeld = false;
            if (bId == 0)
            {
                window.close();
            }
            if (bId == 1)
            {
                unsigned int* t = gen_random_map(tracts, 18, 0.01, pop_diff_m);
                free(t);
                for(int i = 0; i < tracts.size() && !voterColors; i++)
                {
                    testPoints[i].shape.setFillColor(districtColor[tracts[i].district]);
                }
            }
        }
        else if (buttonHeld && !GUIButton && !testPoints[bId].isClickDown(window))
        {
            temp.str(string());
            temp << bId;
            logUpdate = true;
            logNew = "Clicked on testObject no. "+temp.str();
            buttonHeld = false;
        }

        if (logUpdate)
        {
            for (int i = 0; i < logs.size()-1; i++)
            {
                logs[i].setString(logs[i+1].getString());
            }
            //logs[logs.size()-1].setString(String(logNew));
            logUpdate = false;
        }

        for (int i = 0; i < logs.size(); i++)
        {
            window.draw(logs[i]);
        }
        //====================================================
        window.display();
    }

    return 0;
}
