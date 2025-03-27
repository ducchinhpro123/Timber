#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace sf;

void adjust_local_bounds(Text &text);
void update_branches(int seed);

const int NUM_BRANCHES = 6;
Sprite sprite_branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branch_positions[NUM_BRANCHES];

void update_branches(int seed)
{
    for (int i = NUM_BRANCHES - 1; i > 0; i--) {
        branch_positions[i] = branch_positions[i - 1];
    }

    srand((int)time(0) + seed);
    int r = rand() % 6;
    switch (r) {
    case 0:
        branch_positions[0] = side::LEFT;
        break;
    case 1:
        branch_positions[0] = side::RIGHT;
        break;
    default:
        branch_positions[0] = side::NONE;
        break;
    }
}
// Make the give text object center
void adjust_local_bounds(Text &text)
{
    FloatRect text_rect = text.getLocalBounds();
    /* This is what local bounds looks like
     +--------------------+
     |       Hello World  |
     +--------------------+
     * */
    text.setOrigin(text_rect.left + text_rect.width / 2.0, text_rect.top + text_rect.height / 2.0);
    text.setPosition(1920.0f / 2.0, 1080 / 2.0f);
}

int main()
{
    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "SFML Works");
    CircleShape shape(100.f);
    shape.setFillColor(Color::Green);

    Font font;
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    int score = 0;

    SoundBuffer chop_buffer;
    chop_buffer.loadFromFile("sound/chop.wav");
    Sound sound_chop;
    sound_chop.setBuffer(chop_buffer);

    SoundBuffer death_buffer;
    death_buffer.loadFromFile("sound/death.wav");
    Sound sound_death;
    sound_death.setBuffer(death_buffer);

    SoundBuffer out_of_time_buffer;
    out_of_time_buffer.loadFromFile("sound/out_of_time.wav");
    Sound out_of_time_sound;
    out_of_time_sound.setBuffer(out_of_time_buffer);

    side player_side = side::LEFT; // init player's side

    Text message_text;
    Text score_text;

    message_text.setString("Press enter to start!");
    message_text.setCharacterSize(75);
    message_text.setFillColor(Color::White);
    message_text.setFont(font);

    adjust_local_bounds(message_text);

    score_text.setFont(font);
    score_text.setString("Score: 0");
    score_text.setPosition(20, 20);
    score_text.setCharacterSize(100);

    // Declare texture
    Texture texture_background;
    Texture texture_tree;
    Texture texture_bee;
    Texture texture_cloud;
    Texture texture_player;
    Texture texture_branch;
    Texture texture_rip;
    Texture texture_axe;
    Texture texture_log;
    Texture player_dead;
    std::vector<Texture> girl_character(10);
    std::vector<Texture> character_dead(10);
    std::vector<Texture> character_melee(7);

    // Associate texture with the files
    texture_background.loadFromFile("graphics/background.png");
    texture_tree.loadFromFile("graphics/tree.png");
    texture_bee.loadFromFile("graphics/bee.png");
    texture_cloud.loadFromFile("graphics/cloud.png");
    texture_player.loadFromFile("graphics/player.png");
    texture_branch.loadFromFile("graphics/branch.png");
    texture_rip.loadFromFile("graphics/rip.png");
    texture_axe.loadFromFile("graphics/28.png");
    texture_log.loadFromFile("graphics/log.png");
    for (int i = 1; i <= 10; i++) {
        std::string file_name = "graphics/player/Idle(" + std::to_string(i) + ").png";
        if (!girl_character[i - 1].loadFromFile(file_name)) {
            return -1;
        }
    }
    for (int i = 1; i <= 7; i++) {
        std::string file_name = "graphics/player/Melee(" + std::to_string(i) + ").png";
        if (!character_melee[i - 1].loadFromFile(file_name)) {
            return -1;
        }
    }
    for (int i = 1; i <= 10; i++) {
        std::string file_name = "graphics/player/Dead(" + std::to_string(i) + ").png";
        if (!character_dead[i - 1].loadFromFile(file_name)) {
            return -1;
        }
    }

    for (int i = 0; i < NUM_BRANCHES; i++) {
        sprite_branches[i].setTexture(texture_branch);
        sprite_branches[i].setPosition(-2000, -2000);
        sprite_branches[i].setOrigin(220, 20);
    }

    Sprite girl_sprite;
    girl_sprite.setTexture(girl_character[0]);
    girl_sprite.setScale(0.5, 0.5);
    FloatRect bounds_player = girl_sprite.getLocalBounds();
    girl_sprite.setOrigin(bounds_player.width / 2.f, bounds_player.height / 2.f);
    // girl_sprite.setPosition(580, 1000);

    Sprite sprite_axe;
    sprite_axe.setTexture(texture_axe);
    sprite_axe.setPosition(850, 770);
    sprite_axe.setScale(0.2, 0.2);

    Sprite sprite_log;
    sprite_log.setTexture(texture_log);
    sprite_log.setPosition(810, 720);

    Sprite sprite_rip;
    sprite_rip.setTexture(texture_rip);
    sprite_rip.setPosition(675, 830);

    Sprite sprite_player;
    sprite_player.setTexture(texture_player);
    sprite_player.setPosition(580, 720);

    Sprite sprite_background;
    sprite_background.setTexture(texture_background);
    sprite_background.setPosition(0, 0);

    Sprite sprite_tree;
    sprite_tree.setTexture(texture_tree);
    sprite_tree.setPosition(810, 0);

    Sprite sprite_tree1;
    sprite_tree1.setTexture(texture_tree);
    sprite_tree1.setPosition(100, 500);
    sprite_tree1.setScale(0.2, 0.2);

    Sprite sprite_bee;
    sprite_bee.setTexture(texture_bee);
    sprite_bee.setPosition(0, 800);

    float bee_speed = 0.0f;
    bool bee_active = false;
    float bee_angle = 0.0f;
    float bee_original_y = 0.0f;

    Sprite clouds[3];
    float clouds_speed[3];
    float clouds_active[3];

    for (int i = 0; i < 3; i++) {
        Sprite cloud;
        cloud.setTexture(texture_cloud);
        cloud.setPosition(0, i * 250);

        clouds[i] = cloud;
        clouds_speed[i] = 0.0f;
        clouds_active[i] = false;
    }

    bool paused = true;
    // Clock
    Clock clock;
    // Time bar to measure remaining time
    RectangleShape time_bar;
    float time_bar_width = 480;
    float time_bar_height = 40;
    time_bar.setSize(Vector2f(time_bar_width, time_bar_height));
    time_bar.setFillColor(Color::Red);
    time_bar.setPosition((1920.f / 2.f) - (time_bar_width / 2.f), 980);

    float time_remaining = 6.0f;
    float time_bar_per_second = time_bar_width / time_remaining;

    // const float AXE_POSITION_LEFT = 700;
    // const float AXE_POSITION_RIGHT = 1075;
    const float AXE_POSITION_LEFT = 850;
    const float AXE_POSITION_RIGHT = 1100;

    bool log_active = false;
    float log_speed_x = 1000;
    float log_speed_y = -1500;

    int current_frame = 0;
    float frame_time = 0.1;
    float time_since_last_frame = 0.0f;
    float dead_frame_time = 0.1f;
    float dead_time_since_last_frame = 0.0f;
    int dead_frame = 0;

    // float frame_time_melee = 0.05f;
    float melee_frame_time = 0.02f;
    float melee_time_since_last_frame = 0.0f;
    int melee_frame = 0;

    bool game_over = false;
    bool is_attacking = false;

    while (window.isOpen()) {
        Event event;
        Time dt = clock.restart();
        if (!paused) {
            if (!game_over && is_attacking) {
                melee_time_since_last_frame += dt.asSeconds();
                if (melee_time_since_last_frame >= melee_frame_time) {
                    melee_frame++;
                    if (melee_frame >= 7) {
                        melee_frame = 0;
                        is_attacking = false;
                        girl_sprite.setTexture(girl_character[current_frame]);
                    } else {
                        girl_sprite.setTexture(character_melee[melee_frame]);
                    }
                    melee_time_since_last_frame = 0;
                }
            }

            /* Update girl charater */
            if (!game_over && !is_attacking) { // Update idle state frames
                time_since_last_frame += dt.asSeconds();
                if (time_since_last_frame >= frame_time) {
                    current_frame = (current_frame + 1) % 10;
                    girl_sprite.setTexture(girl_character[current_frame]);
                    time_since_last_frame = 0.0f;
                }
            }

            /* Update time bar */
            time_remaining -= dt.asSeconds();
            time_bar.setSize(Vector2f(time_remaining * time_bar_per_second, time_bar_height));

            /* Update branches */
            for (int i = 0; i < NUM_BRANCHES; i++) {
                float branch_y_pos = i * 150.f;
                switch (branch_positions[i]) {
                case side::LEFT:
                    sprite_branches[i].setPosition(610, branch_y_pos);
                    sprite_branches[i].setRotation(180);
                    break;
                case side::RIGHT:
                    sprite_branches[i].setPosition(1330, branch_y_pos);
                    sprite_branches[i].setRotation(0);
                    break;
                case side::NONE:
                    sprite_branches[i].setPosition(3000, branch_y_pos);
                    break;
                }
            }

            if (log_active) {
                sprite_log.setPosition(sprite_log.getPosition().x + (log_speed_x * dt.asSeconds()),
                                       sprite_log.getPosition().y + (log_speed_y * dt.asSeconds()));

                if (sprite_log.getPosition().x < -100 || sprite_log.getPosition().x > 2000) {
                    log_active = false;
                    sprite_log.setPosition(810, 720);
                }
            }

            if (branch_positions[5] == player_side) {
                if (!game_over) {
                    game_over = true;
                    sprite_rip.setPosition(830, 830);
                    message_text.setString("SQUISHED, YOU'RE STUPID AS F***");
                    adjust_local_bounds(message_text);

                    dead_time_since_last_frame = 0.0f;
                    dead_frame = 0;
                    sound_death.play();
                }
                // paused = true;

                dead_time_since_last_frame += dt.asSeconds();
                if (dead_time_since_last_frame >= dead_frame_time) { // Update dead state frames
                    dead_frame = (dead_frame + 1) % 10;
                    girl_sprite.setTexture(character_dead[dead_frame]);
                    dead_time_since_last_frame = 0.0f;

                    if (dead_frame == 9) {
                        paused = true;
                    }
                }
            }

            /* BEE */
            if (!bee_active) {
                srand((int)time(0));
                bee_speed = (rand() % 200) + 200; // random number from 200 to 399

                srand((int)time(0) * 10);
                float y_bee_pos = (rand() % 500) + 500;  // between 500 and 999
                sprite_bee.setPosition(2000, y_bee_pos); // 2000 is just off-screen to the right

                bee_original_y = y_bee_pos;
                bee_angle = 0.0f;
                bee_active = true;

            } else {
                bee_angle += rand() % 20 * dt.asSeconds();
                float vertical_offset = sin(bee_angle) * 30.0f; // bee is motion like a sine wave
                sprite_bee.setPosition(sprite_bee.getPosition().x - (bee_speed * dt.asSeconds()),
                                       bee_original_y + vertical_offset);

                if (sprite_bee.getPosition().x < -100) {
                    bee_active = false;
                }
            }

            /* CLOUDS */
            for (size_t i = 0; i < 3; i++) {
                if (!clouds_active[i]) {
                    srand((int)time(0) * (i + 1) * 10);

                    int dir = (rand() % 2 == 0) ? 1 : -1;
                    clouds_speed[i] = dir * ((rand() % 200) + 200);
                    float x_cloud_pos;

                    if (dir > 0) {
                        x_cloud_pos = -300;
                    } else {
                        x_cloud_pos = 1920 + 300;
                    }

                    float y_cloud_pos = (rand() % 150);
                    clouds[i].setPosition(x_cloud_pos, y_cloud_pos);
                    clouds_active[i] = true;

                } else {
                    float cloud_current_pos = clouds[i].getPosition().x;
                    clouds[i].setPosition(cloud_current_pos + (dt.asSeconds() * clouds_speed[i]),
                                          clouds[i].getPosition().y);

                    if (cloud_current_pos < 0 && cloud_current_pos < -300 ||
                        cloud_current_pos > 0 && cloud_current_pos > 1920 + 300) {
                        clouds_active[i] = false;
                    }
                }
            }

        } // End if (!paused)

        if (time_remaining <= 0.0f) {
            paused = true;
            message_text.setString("Out of time!");

            adjust_local_bounds(message_text);
            out_of_time_sound.play();
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Return) {
                paused = false;
                game_over = false;
                score = 0; // Reset score
                time_remaining = 6.0f;

                for (int i = 1; i < NUM_BRANCHES; i++) {
                    branch_positions[i] = side::NONE;
                }
                sprite_rip.setPosition(675, 2000);

                girl_sprite.setPosition(750, 780);
            }
            if (!paused && !game_over && !is_attacking) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Right) {
                    melee_time_since_last_frame = 0.f;
                    is_attacking = true;

                    player_side = side::RIGHT;
                    score++;
                    time_remaining += (2.f / score) + .15;

                    sprite_axe.setPosition(AXE_POSITION_RIGHT, sprite_axe.getPosition().y);
                    sprite_log.setPosition(810, 720);
                    log_speed_x = -5000;
                    log_active = true;

                    girl_sprite.setTexture(character_melee[0]);

                    update_branches(score);
                    sound_chop.play();

                    /* Recording score */
                    std::stringstream ss;
                    ss << "Score: " << score;
                    score_text.setString(ss.str());
                }

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Left) {
                    melee_time_since_last_frame = 0.f;
                    is_attacking = true;
                    player_side = side::LEFT;

                    score++;
                    time_remaining += (2.f / score) + .15;

                    sprite_axe.setPosition(AXE_POSITION_LEFT, sprite_axe.getPosition().y);
                    sprite_log.setPosition(810, 720);
                    log_speed_x = 5000;
                    log_active = true;

                    girl_sprite.setTexture(character_melee[0]);

                    update_branches(score);
                    sound_chop.play();

                    /* Recording score */
                    std::stringstream ss;
                    ss << "Score: " << score;
                    score_text.setString(ss.str());
                }
            }
        }

        switch (player_side) {
        case side::LEFT:
            girl_sprite.setPosition(750, 780);
            girl_sprite.setScale(0.5f, 0.5f);
            // sprite_axe.setScale(-0.2, 0.2);
            break;
        case side::RIGHT:
            girl_sprite.setPosition(1180, 780);
            girl_sprite.setScale(-0.5f, 0.5f);
            // sprite_axe.setScale(0.2, 0.2);
            break;
        case side::NONE:
            break;
        }

        window.clear();
        window.draw(sprite_background);

        for (const auto &cloud : clouds) {
            window.draw(cloud);
        }

        window.draw(sprite_tree1);
        window.draw(sprite_tree);
        // window.draw(sprite_player);

        window.draw(sprite_log);
        window.draw(girl_sprite);
        // window.draw(sprite_axe);
        window.draw(sprite_bee);
        window.draw(sprite_rip);
        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(sprite_branches[i]);
        }
        if (paused) {
            window.draw(message_text);
        }
        window.draw(time_bar);
        window.draw(score_text);
        window.display();
    }

    return 0;
}
