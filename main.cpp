#include <SFML/Graphics.hpp>
#include <time.h>
#include <string>
#include <sstream>
using namespace sf;

int main()
{
    srand(time(0));

    // Увеличенное окно
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid HD!");
    app.setFramerateLimit(60);

    // Загрузка текстур
    Texture t1, t2, t3, t4;
    if (!t1.loadFromFile("images/block01.png")) return EXIT_FAILURE;
    if (!t2.loadFromFile("images/background.jpg")) return EXIT_FAILURE;
    if (!t3.loadFromFile("images/ball.png")) return EXIT_FAILURE;
    if (!t4.loadFromFile("images/paddle.png")) return EXIT_FAILURE;

    // Масштабируем фон под новый размер окна
    t2.setRepeated(true);
    Sprite sBackground(t2);
    sBackground.setTextureRect(IntRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    // Увеличиваем шарик и платформу
    Sprite sBall(t3), sPaddle(t4);
    sBall.setScale(1.5f, 1.5f); // Увеличиваем шарик в 1.5 раза
    sPaddle.setScale(1.5f, 1.0f); // Шире, но не выше

    // Начальные позиции
    sPaddle.setPosition(WINDOW_WIDTH / 2 - sPaddle.getGlobalBounds().width / 2, WINDOW_HEIGHT - 50);
    sBall.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 125);

    // Шрифт
    Font font;
    if (!font.loadFromFile("fonts/arial.ttf")) return EXIT_FAILURE;

    // Блоки (15x10)
    const int BLOCK_ROWS = 10;
    const int BLOCK_COLS = 16;
    const float BLOCK_WIDTH = 50.0f;
    const float BLOCK_HEIGHT = 25.0f;
    Sprite blocks[BLOCK_ROWS * BLOCK_COLS];

    int n = 0;
    for (int i = 0; i < BLOCK_COLS; i++)
        for (int j = 0; j < BLOCK_ROWS; j++)
        {
            blocks[n].setTexture(t1);
            blocks[n].setPosition(i * (BLOCK_WIDTH - 2) + 10, j * (BLOCK_HEIGHT - 2) + 50);
            blocks[n].setScale(1.1f, 1.1f); // Чуть увеличим блоки
            n++;
        }

    // Игровые переменные
    float ballSpeedX = 5.0f;
    float ballSpeedY = -5.0f;
    int score = 0;
    int lives = 3;
    bool gameOver = false;
    bool win = false;
    bool paused = false;
    bool startScreen = true;

    // Основной цикл
    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed) app.close();

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::P) paused = !paused;
                if (e.key.code == Keyboard::R) // Рестарт
                {
                    ballSpeedX = 5.0f;
                    ballSpeedY = -5.0f;
                    sBall.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 125);
                    sPaddle.setPosition(WINDOW_WIDTH / 2 - sPaddle.getGlobalBounds().width / 2, WINDOW_HEIGHT - 50);
                    score = 0;
                    lives = 3;
                    gameOver = false;
                    win = false;

                    // Восстанавливаем блоки
                    n = 0;
                    for (int i = 0; i < BLOCK_COLS; i++)
                        for (int j = 0; j < BLOCK_ROWS; j++)
                        {
                            blocks[n].setPosition(i * (BLOCK_WIDTH - 2) + 10, j * (BLOCK_HEIGHT - 2) + 50);
                            blocks[n].setScale(1.1f, 1.1f);
                            n++;
                        }
                }
                if (e.key.code == Keyboard::Space && startScreen) startScreen = false;
            }
        }

        // Начальный экран
        if (startScreen)
        {
            Text title("ARKANOID", font, 60);
            title.setPosition(WINDOW_WIDTH / 2 - title.getGlobalBounds().width / 2, 150);
            title.setFillColor(Color::Cyan);

            Text pressSpace("Press SPACE to start \n R - Restart \n P - Pause \n You'll only have 3 lives!", font, 30);
            pressSpace.setPosition(WINDOW_WIDTH / 2 - pressSpace.getGlobalBounds().width / 2, 300);
            pressSpace.setFillColor(Color::Yellow);

            app.clear();
            app.draw(sBackground);
            app.draw(title);
            app.draw(pressSpace);
            app.display();
            continue;
        }

        // Пауза
        if (paused)
        {
            Text pauseText("PAUSED (Press P to continue)", font, 40);
            pauseText.setPosition(WINDOW_WIDTH / 2 - pauseText.getGlobalBounds().width / 2, 250);
            pauseText.setFillColor(Color::White);

            app.draw(pauseText);
            app.display();
            continue;
        }

        // Игра окончена или победа
        if (gameOver || win)
        {
            Text endText;
            if (gameOver) endText.setString("GAME OVER! Press R to restart");
            else endText.setString("YOU WIN! Score: " + std::to_string(score) + " Press R to restart");

            endText.setFont(font);
            endText.setCharacterSize(40);
            endText.setPosition(WINDOW_WIDTH / 2 - endText.getGlobalBounds().width / 2, 250);
            endText.setFillColor(gameOver ? Color::Red : Color::Green);

            app.draw(endText);
            app.display();
            continue;
        }

        // Движение платформы
        float paddleSpeed = 10.0f;
        if (Keyboard::isKeyPressed(Keyboard::Right) && sPaddle.getPosition().x < WINDOW_WIDTH - sPaddle.getGlobalBounds().width)
            sPaddle.move(paddleSpeed, 0);
        if (Keyboard::isKeyPressed(Keyboard::Left) && sPaddle.getPosition().x > 0)
            sPaddle.move(-paddleSpeed, 0);

        // Движение шарика
        sBall.move(ballSpeedX, ballSpeedY);
        FloatRect ballBounds = sBall.getGlobalBounds();

        // Столкновение с блоками
        bool allBlocksGone = true;
        for (int i = 0; i < n; i++)
        {
            if (blocks[i].getPosition().y > 0)
            {
                allBlocksGone = false;
                if (ballBounds.intersects(blocks[i].getGlobalBounds()))
                {
                    blocks[i].setPosition(-100, 0); // Удаляем блок
                    ballSpeedY = -ballSpeedY; // Меняем направление
                    score += 10;
                }
            }
        }

        // Проверка победы
        if (allBlocksGone) win = true;

        // Столкновение с границами
        if (ballBounds.left <= 0 || ballBounds.left + ballBounds.width >= WINDOW_WIDTH)
            ballSpeedX = -ballSpeedX;
        if (ballBounds.top <= 0)
            ballSpeedY = -ballSpeedY;

        // Промах (шарик улетел вниз)
        if (ballBounds.top + ballBounds.height >= WINDOW_HEIGHT)
        {
            lives--;
            if (lives <= 0) gameOver = true;
            else
            {
                // Возвращаем шарик и платформу в центр
                sBall.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 125);
                sPaddle.setPosition(WINDOW_WIDTH / 2 - sPaddle.getGlobalBounds().width / 2, WINDOW_HEIGHT - 50);
                ballSpeedX = 5.0f * (rand() % 2 == 0 ? 1 : -1); // Случайное направление
                ballSpeedY = -5.0f;
            }
        }

        // Столкновение с платформой
        if (ballBounds.intersects(sPaddle.getGlobalBounds()))
        {
            // Зависимость отражения от места удара
            float hitPos = (sBall.getPosition().x + sBall.getGlobalBounds().width / 2) - sPaddle.getPosition().x;
            float normalizedHitPos = hitPos / sPaddle.getGlobalBounds().width;
            ballSpeedX = normalizedHitPos * 8.0f; // Чем ближе к краю, тем сильнее отскок
            ballSpeedY = -fabs(ballSpeedY); // Гарантированно вверх
        }

        // Отрисовка
        app.clear();
        app.draw(sBackground);
        app.draw(sBall);
        app.draw(sPaddle);

        // Отрисовка блоков
        for (int i = 0; i < n; i++)
            if (blocks[i].getPosition().y > 0)
                app.draw(blocks[i]);

        // Отрисовка счёта и жизней
        Text scoreText("Score: " + std::to_string(score), font, 30);
        scoreText.setPosition(20, 20);
        scoreText.setFillColor(Color::White);

        Text livesText("Lives: " + std::to_string(lives), font, 30);
        livesText.setPosition(WINDOW_WIDTH - livesText.getGlobalBounds().width - 20, 20);
        livesText.setFillColor(Color::White);

        app.draw(scoreText);
        app.draw(livesText);
        app.display();
    }

    return 0;
}