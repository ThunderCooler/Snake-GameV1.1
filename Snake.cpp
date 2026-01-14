#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <random>
#include <string>
#include <iostream>
using namespace sf;
using namespace std;

namespace
{
std::filesystem::path resourcesDir()
{
#ifdef SFML_SYSTEM_IOS
    return "";
#else
    return "resources";
#endif
}
} // namespace

std::random_device rd;
std::mt19937 gen(rd());

SoundBuffer ballSoundBuffer;
Sound ballSound(ballSoundBuffer);

int winpoints = 99;
float maxX = 1200.f;
float maxY = 800.f;

void CreateExtraBalls(CircleShape &B) {
    B.setPosition(Vector2f(-200.f,-200.f));
    B.setOutlineThickness(5);
    B.setOutlineColor(Color::Black);
}

void RandBall(CircleShape &B2) {
    uniform_real_distribution<float> dist(40.f,maxX-40);
    uniform_real_distribution<float> dist2(40.f,maxY-40);
    B2.setPosition(Vector2f(dist(gen), dist2(gen)));
}

bool BallCollide(CircleShape &B, RectangleShape Snake) {
    bool collide = false;
    if (B.getGlobalBounds().findIntersection(Snake.getGlobalBounds())) {
        collide = true;
        ballSound.play();
    }
    return collide;
}

Vector2f Movement(bool &l, bool &r, bool &u, bool &d, RectangleShape ob, float spd ) {
    if (l) ob.move(Vector2f(-spd,0));
    if (r) ob.move(Vector2f(spd,0));
    if (u) ob.move(Vector2f(0,-spd));
    if (d) ob.move(Vector2f(0,spd));
    return ob.getPosition();
}

int main() {

    RenderWindow window(VideoMode(Vector2u(maxX, maxY)),"");

    Clock clock;
    Clock clock2;
    
    RectangleShape Array[111];
    for(int c = 0; c < 111; c++){
        Array[c].setPosition(Vector2f(-100.f,-100.f));
        Array[c].setFillColor(Color::Green);
        Array[c].setOutlineThickness(5);
        Array[c].setOutlineColor(Color::Black);
        Array[c].setSize(Vector2f(30.f,30.f));
    }
    if (!ballSoundBuffer.loadFromFile(resourcesDir() / "ball.wav")) {
    std::cerr << "Failed to load ball.wav\n";
    }
    ballSound.setVolume(70.f);

    RectangleShape Snake(Vector2f(30.f , 30.f));
    Snake.setPosition(Vector2f(600.f,400.f));
    Snake.setFillColor(Color(10,80,40));
    Snake.setOutlineThickness(5);
    Snake.setOutlineColor(Color::Black);

    CircleShape Ball(20.f), Spdball(15.f), Bonusball(15.f), Bomb(30.f), Ball2(20.f), Potion(10.f);
    Ball.setPosition(Vector2f(300.f,240.f));
    Ball.setOutlineThickness(5);
    Ball.setOutlineColor(Color::Black);
    Ball.setFillColor(Color::White);

    CreateExtraBalls(Ball2);
    Ball2.setFillColor(Color::White);

    CreateExtraBalls(Potion);
    Potion.setFillColor(Color::Magenta);

    CreateExtraBalls(Spdball);
    Spdball.setFillColor(Color::Cyan);

    CreateExtraBalls(Bonusball);
    Bonusball.setFillColor(Color::Yellow);

    CreateExtraBalls(Bomb);
    Bomb.setFillColor(Color::Red);

    Font font;
    font.openFromFile("C:\\Windows\\Fonts\\arial.ttf");

    Text points(font), Lose(font), WinText(font);
    points.setString("0");
    points.setCharacterSize(64);
    points.setFillColor(Color::White);
    points.setOutlineThickness(5);
    points.setOutlineColor(Color::Black);
    points.setPosition(Vector2f(maxX/2,30.f));

    Lose.setString("You Lost");
    Lose.setCharacterSize(128);
    Lose.setFillColor(Color::White);
    Lose.setPosition(Vector2f(350,300));

    WinText.setString("You Win!");
    WinText.setCharacterSize(128);
    WinText.setFillColor(Color::White);
    WinText.setPosition(Vector2f(350,300));

    int pointsCounter = 0, chance = 0, speedCount, PotionAffectCount = 0, red = 75, green = 75, blue = 75, steps = 0;
    float speed = 40.f, seconds = 0.1f, position[100000][2], distanceApart = 100.f, distanceFromBorder = 100.f;
    long int counter = 0;
    bool right = false, left = true, up = false, down = false, paused = false, pauseStarted = false, spawned = false, IsDrankPotion = false, lose, win, Isbomb = false;

    while(window.isOpen()) {
            window.clear(Color(red,green,blue));
                if (pointsCounter > 0) {
                    for(int i = 0; i < pointsCounter; i++) {
                    window.draw(Array[i]);
                    Array[i].setPosition(Vector2f(position[i][0],position[i][1]));
                        if(Array[i].getGlobalBounds().findIntersection(Snake.getGlobalBounds())) {
                            lose = true;
                        }
                    }
                }
                if ((lose || win) && !pauseStarted) {
                    paused = true;
                    pauseStarted = true;
                    clock2.restart();   // start timer ONCE
                }
                while (paused && clock2.getElapsedTime().asSeconds() < 2.f) {
                window.clear(Color::Black);
                if (lose) window.draw(Lose);
                else if (win) {
                    window.draw(WinText);
                }
                window.display();

                if (clock2.getElapsedTime().asSeconds() >= 2.f) {
                    // reset game
                    pointsCounter = 0, chance = 0, speedCount = 60, PotionAffectCount = 0, steps = 0;
                    speed = 40.f, position[100000][2], distanceApart = 100.f, distanceFromBorder = 100.f;
                    counter = 0;
                    right = false, left = true, up = false, down = false, spawned = false, IsDrankPotion = false, Isbomb = false;

                    for(int i = 0; i < 99; i++)
                        Array[i].setPosition(Vector2f(-100.f,-100.f));

                    Snake.setPosition(Vector2f(600.f,450.f));
                    Ball.setPosition(Vector2f(300.f,240.f));
                    Spdball.setPosition(Vector2f(-100.f,-100.f));
                    Bonusball.setPosition(Vector2f(-100.f,-100.f));
                    Bomb.setPosition(Vector2f(-1000.f,-1000.f));
                    Potion.setPosition(Vector2f(-100.f,-100.f));
                    Ball2.setPosition(Vector2f(-100.f,-100.f));
                    points.setString("0");
                    Bomb.setRadius(20.f);

                }
                }
                    paused = false;
                    pauseStarted = false;


                if (!paused) {
                window.draw(Snake);
                window.draw(Bomb);
                window.draw(Spdball);
                window.draw(Bonusball);
                window.draw(Potion);
                window.draw(Ball);
                window.draw(Ball2);
                window.draw(points);
                window.display();
                }
            
            lose = false;
            win = false;

            if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) window.close();

            if(clock.getElapsedTime().asSeconds() >= seconds) {

                double xcord = Snake.getPosition().x;
                double ycord = Snake.getPosition().y;

                if (xcord <= -40.f) Snake.setPosition(Vector2f(maxX, Snake.getPosition().y));
                else if (xcord >= maxX) Snake.setPosition(Vector2f( -40.f, Snake.getPosition().y));
                if (ycord <= -40.f) Snake.setPosition(Vector2f(Snake.getPosition().x, maxY));
                else if (ycord >= maxY) Snake.setPosition(Vector2f(Snake.getPosition().x, -40.f ));
                
                if (BallCollide(Ball,Snake) || BallCollide(Ball2,Snake)) {
                    uniform_int_distribution<int> dist3(0,9);
                    chance = dist3(gen);
                    if (BallCollide(Ball,Snake)) RandBall(Ball);
                    if (BallCollide(Ball2,Snake) && IsDrankPotion) RandBall(Ball2);
                    pointsCounter++;
                }
                if ((chance >= 1 && chance <= 7) && !spawned) {
                    if (chance == 5 || chance == 6)  RandBall(Spdball);
                    if (chance == 3) RandBall(Bonusball);
                    if (chance == 1 || chance == 2 || chance == 4) {
                        uniform_real_distribution<float> bdist(distanceFromBorder,xcord-distanceApart);
                        uniform_real_distribution<float> bdist2(distanceFromBorder,ycord-distanceApart);
                        if (xcord <= 600.f) bdist = uniform_real_distribution<float>(xcord + distanceApart ,maxX-distanceFromBorder);
                        if (ycord <= 400.f) bdist2 = uniform_real_distribution<float>(ycord + distanceApart ,maxY-distanceFromBorder);
                        Bomb.setPosition(Vector2f(bdist(gen), bdist2(gen)));
                        Bomb.setRadius(30.f + pointsCounter*3.f);
                        distanceApart -= 5.f;
                        distanceFromBorder += 5.f;
                        if (distanceApart <= 50.f) distanceApart = 50.f;
                        if (distanceFromBorder >= 200.f) distanceFromBorder = 200.f;
                        steps = 0;
                        Isbomb = true;
                    }
                    else if (chance == 7) RandBall(Potion); 
                    spawned = true;
                }

                else if (chance == 0 || chance > 7) {
                    Spdball.setPosition(Vector2f(-100.f,-100.f));
                    Bonusball.setPosition(Vector2f(-100.f,-100.f));
                    Potion.setPosition(Vector2f(-100.f,-100.f));
                    spawned = false;
                }

                if (BallCollide(Spdball,Snake) && spawned) {
                    speed = 60.f;
                    Spdball.setPosition(Vector2f(-100.f,-100.f));
                    chance = 0;
                    speedCount = 0;
                    spawned = false;
                }

                if (BallCollide(Bonusball,Snake) && spawned) {
                    pointsCounter +=5;
                    Spdball.setPosition(Vector2f(-100.f,-100.f));
                    chance = 0;
                    spawned = false;
                }

                if (BallCollide(Bomb,Snake) && spawned) {
                    lose = true;
                    Bomb.setPosition(Vector2f(-1000.f,-1000.f));
                    chance = 0;
                    spawned = false;
                }
                
                if (BallCollide(Potion,Snake) && spawned) {
                    IsDrankPotion = true;
                    RandBall(Ball2);
                    Potion.setPosition(Vector2f(-100.f,-100.f));
                    chance = 0;
                    spawned = false;
                }

                if (IsDrankPotion) {
                    PotionAffectCount++;
                    if (PotionAffectCount >= 300) {
                        PotionAffectCount = 0;
                        IsDrankPotion = false;
                        Ball2.setPosition(Vector2f(-100.f,-100.f));
                        spawned = false;
                    }
                }

                if (Isbomb) {
                    steps++;
                    if (steps >= 100) {
                        steps = 0;
                        Bomb.setPosition(Vector2f(-1000.f,-1000.f));
                        Isbomb = false;
                        spawned = false;
                    }
                } else steps = 0;

                if (pointsCounter >= winpoints) win = true;
                speedCount++;

                if (speedCount >= 60) speed = 40.f;

                if (counter > 0) {
                    if (counter == 100000) counter = 1;
                    for (int j = counter; j > 0; j--) {
                    position[j][0] = position[j-1][0];
                    position[j][1] = position[j-1][1];
                    }
                }
                position[0][0] = xcord;
                position[0][1] = ycord;
                counter++;

                     if (Keyboard::isKeyPressed(Keyboard::Key::Right)) right = true, left = false, up = false, down = false;
                else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) right = false, left = true, up = false, down = false;
                else if (Keyboard::isKeyPressed(Keyboard::Key::Up)) right = false, left = false, up = true, down = false;
                else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) right = false, left = false, up = false, down = true;

                Snake.setPosition(Movement(left,right,up,down,Snake,speed));
                clock.restart();
            }
                points.setString(to_string(pointsCounter));
            }
    return 0;

}
