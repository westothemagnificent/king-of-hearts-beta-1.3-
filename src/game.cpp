#include "SDL.h"
#include "SDL_ttf.h"
#include "vec_ex_.hpp"
#include <Windows.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

bool keep_window_open = true;
// game vars
int cards             = 6;
int cards_left        = cards;
const int health_max  = 20;
int max_health        = health_max;
int health            = max_health;
int won               = -1;
bool load_level       = true;
bool moved_this_frame = false;
int shake_frame       = 0;
int moves             = 0;
int new_cenx          = 0;
int new_ceny          = 0;
int random_card_index = -1;
int heart;
std::vector<int> card_sprites_indexs            = {};
std::vector<int> cards_sprites_des_frames       = {};
std::vector<std::pair<int, int>> sp_card_indexs = { { 0, 50 }, { 1, 90 } };
std::vector<std::vector<std::string>> items     = { { "gold_bar", "art/gold_bar.bmp" },
                                                    { "gold_coin", "art/gold_coin.bmp" } };
std::vector<std::string> collected_items        = { "gold_bar", "gold_coin" };
std::vector<int> collected_items_amount         = { 1, 15 };
// engine vars
int mouseX = 90;
int mouseY = 90;
int width  = 1000;
int height = 500;
int camx;
int camy;

class sprite {
public:
    SDL_Surface * sprite_suf;
    SDL_Texture * sprite_tex;
    SDL_Rect sprite_rec;
    int layer;
    std::string name;
    std::string tag;
};

class button {
public:
    int x;
    int y;
    int w;
    int h;
    std::string tag;
    // 0 = no state, 1 = pressed, 2 = hovered
    int state = 0;
};

class text {
public:
    SDL_Texture * text_tex;
    SDL_Rect text_rec;
};

button button_class;
sprite sprite_class_tem;
text text_class_tem;

std::vector<sprite> hearts        = {};
std::vector<sprite> cards_sprites = {};
std::vector<sprite> spritesnc     = {};
std::vector<text> texts           = {};
sprite win_lose_ui;
sprite hand;
sprite player;
sprite random_card;
std::vector<int> all_sprites_in_order = {};
std::vector<button> buttons           = {};

int random(int lb, int ub) {
    int r = rand() % ub;
    if (rand() % ub == 1) {
        r = ub;
    } else {
        if (r < lb) {
            r += lb;
        }
    }
    return (r);
}

SDL_Window * open_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cout << "Failed to initialize the SDL2 library\n";
        std::cout << "error: \n" << SDL_GetError();
        std::terminate();
    }
    SDL_Window * window = SDL_CreateWindow(
        "king of hearts",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    return window;
}
SDL_Surface * get_suf(SDL_Window * window) {
    SDL_Surface * window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        std::cout << "Failed to get the surface from the window\n";
        std::cout << "error: \n" << SDL_GetError();
        std::terminate();
    }
    return window_surface;
}

void check_if_user_exit_then_exit(SDL_Event e) {
    if (keep_window_open) {
        switch (e.type) {
            case SDL_QUIT:
                keep_window_open = false;
                std::cout << "SDL closed";
                break;
        }
    }
}
text create_text(
    SDL_Renderer * ren, const char * textr, int size, SDL_Color rgb, TTF_Font * Sans, int x, int y
) {
    std::string textstr = textr;
    if (!Sans) {
        std::cout << "Failed to load font\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_Surface * image = TTF_RenderText_Solid(Sans, textr, rgb);
    if (!image) {
        std::cout << "Failed to load image\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_Rect rect;
    rect.w = textstr.length() * (size + 20);
    rect.h = size * 10;
    rect.x = x;
    rect.y = y;
    text sprite_class;
    sprite_class.text_tex = SDL_CreateTextureFromSurface(ren, image);
    sprite_class.text_rec = rect;
    if (!sprite_class.text_tex) {
        std::cout << "Failed to make tux\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    return sprite_class;
}
void change_text(
    SDL_Renderer * ren,
    const char * textr,
    int size,
    TTF_Font * Sans,
    SDL_Color rgb,
    text * sprite_class
) {
    std::string textstr = textr;
    SDL_Surface * image = TTF_RenderText_Solid(Sans, textr, rgb);
    if (!image) {
        std::cout << "Failed to load image\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_DestroyTexture(sprite_class->text_tex);
    sprite_class->text_tex = SDL_CreateTextureFromSurface(ren, image);
    if (!sprite_class->text_tex) {
        std::cout << "Failed to make tux\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_FreeSurface(image);
}

sprite create_spite(
    std::shared_ptr<Asset> asset,
    int x,
    int y,
    int w,
    int h,
    std::string name,
    std::string tag,
    int l
) {
    SDL_Surface * image = NULL;
    if (sprite_r != "null") {
        image = SDL_LoadBMP(sprite_r);
    }
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    if (!image && sprite_r != "null") {
        std::cout << "Failed to load image\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    sprite sprite_class;
    sprite_class.layer      = l;
    sprite_class.name       = name;
    sprite_class.tag        = tag;
    sprite_class.sprite_suf = image;
    if (sprite_r != "null") {
        sprite_class.sprite_tex = SDL_CreateTextureFromSurface(ren, image);
    } else {
        sprite_class.sprite_tex = NULL;
    }
    if (!sprite_class.sprite_tex && sprite_r != "null") {
        std::cout << "Failed to make tux\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    sprite_class.sprite_rec = rect;
    sprite_class.layer      = l;
    return sprite_class;
}
void render_sprite(sprite sprite, SDL_Renderer * ren) {
    sprite.sprite_rec.x = (sprite.sprite_rec.x - sprite.sprite_rec.w / 2);
    sprite.sprite_rec.y = (sprite.sprite_rec.y - sprite.sprite_rec.h / 2);
    if (sprite.tag != "ui") {
        sprite.sprite_rec.x += camx;
        sprite.sprite_rec.y += camy;
    }
    SDL_RenderCopy(ren, sprite.sprite_tex, NULL, &sprite.sprite_rec);
}
void change_sprite_width_height(int w, int h, sprite * sprite) {
    sprite->sprite_rec.w = w;
    sprite->sprite_rec.h = h;
}
void change_sprite_x_y(int x, int y, sprite * sprite) {
    sprite->sprite_rec.x = x;
    sprite->sprite_rec.y = y;
}
void change_sprite(const char * file, sprite * spriter, SDL_Renderer * ren) {
    SDL_Surface * suf = SDL_LoadBMP(file);
    SDL_DestroyTexture(spriter->sprite_tex);
    spriter->sprite_tex = SDL_CreateTextureFromSurface(ren, suf);
    if (!spriter->sprite_tex) {
        std::cout << "Failed to make tux\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_FreeSurface(suf);
}
void destory_sprite(sprite * spriter) {
    spriter->sprite_rec = sprite_class_tem.sprite_rec;
    spriter->name       = "";
    spriter->tag        = "";
    SDL_DestroyTexture(spriter->sprite_tex);
    SDL_FreeSurface(spriter->sprite_suf);
    spriter->sprite_suf = NULL;
    spriter->sprite_tex = NULL;
}
void destory_text(text * spriter) {
    spriter->text_rec = text_class_tem.text_rec;
    SDL_DestroyTexture(spriter->text_tex);
    spriter->text_tex = NULL;
}
void create_button(int x, int y, int w, int h, std::string tag) {
    buttons.push_back(button_class);
    int size          = buttons.size() - 1;
    buttons[size].x   = x;
    buttons[size].y   = y;
    buttons[size].w   = w;
    buttons[size].h   = h;
    buttons[size].tag = tag;
}
void handle_buttons(SDL_Event e) {
    for (int i = 0; i < size(buttons); i++) {
        if (buttons[i].tag != "ui") {
            buttons[i].x += camx;
            buttons[i].y += camy;
        }
        if ((e.type == SDL_MOUSEBUTTONDOWN)
            && (((buttons[i].x + buttons[i].w) > mouseX) && ((buttons[i].x - buttons[i].w) < mouseX)
            )
            && (((buttons[i].y + buttons[i].h) > mouseY) && ((buttons[i].y - buttons[i].h) < mouseY)
            ))
        {
            buttons[i].state = 1;
        } else if (
            (((buttons[i].x + buttons[i].w) > mouseX) && ((buttons[i].x - buttons[i].w) < mouseX))
            && (((buttons[i].y + buttons[i].h) > mouseY)
                && ((buttons[i].y - buttons[i].h) < mouseY)))
        {
            buttons[i].state = 2;
        } else {
            buttons[i].state = 0;
        }
        if (buttons[i].tag != "ui") {
            buttons[i].x -= camx;
            buttons[i].y -= camy;
        }
    }
}
std::vector<sprite> add_all_sprites() {
    std::vector<sprite> all_sprites = {};
    all_sprites.push_back(hand);
    all_sprites.push_back(player);
    all_sprites.push_back(random_card);
    all_sprites.push_back(win_lose_ui);
    all_sprites = vec_ex::add_vec_to_vec(all_sprites, spritesnc);
    all_sprites = vec_ex::add_vec_to_vec(all_sprites, hearts);
    all_sprites = vec_ex::add_vec_to_vec(all_sprites, cards_sprites);
    return all_sprites;
}
void rest_orders() {
    std::cout << "reset!\n";
    std::vector<sprite> all_sprites = {};
    all_sprites_in_order            = {};
    all_sprites                     = add_all_sprites();
    std::vector<int> layers_edited  = {};
    for (int i = 0; i < size(all_sprites); i++) {
        layers_edited.push_back(all_sprites[i].layer);
        std::cout << all_sprites[i].layer;
    }
    vec_ex::print_vec(layers_edited);
    for (int i = layers_edited[vec_ex::find_gratest_in_vec(layers_edited)]; i > 0; i--) {
        std::cout << "loop\n";
        for (int index = 0; index < size(all_sprites); index++) {
            if (layers_edited[index] == i) {
                // std::terminate();
                all_sprites_in_order.push_back(index);
                // layers_edited.erase(layers_edited.begin() + index-1);
                // all_sprites.erase(all_sprites.begin() + index-1);
            }
            // vec_ex::print_vec(layers_edited);
        }
    }
}
void render_all_text(SDL_Renderer * ren) {
    std::vector<text> all_sprites = {};
    all_sprites                   = vec_ex::add_vec_to_vec(all_sprites, texts);
    for (int i = 0; i < size(all_sprites); i++) {
        all_sprites[i].text_rec.x -= all_sprites[i].text_rec.w / 2;
        all_sprites[i].text_rec.y -= all_sprites[i].text_rec.h / 2;
        SDL_RenderCopy(ren, all_sprites[i].text_tex, NULL, &all_sprites[i].text_rec);
    }
}
void render_all_sprites(SDL_Renderer * ren) {
    std::vector<sprite> all_sprites = {};
    all_sprites                     = add_all_sprites();
    for (int i = 0; i < all_sprites_in_order.size(); i++) {
        if (all_sprites[all_sprites_in_order[i]].name == "player_hand") {
            render_all_text(ren);
        }
        render_sprite(all_sprites[all_sprites_in_order[i]], ren);
    }
    std::cout << "rendered\n";
}

// scripts

void make_cards(SDL_Renderer * ren, int cards_to_make) {
    int y = 0;
    bool sp_card;
    int r;
    for (int i = 0; i < cards_to_make; i++) {
        std::cout << "begin\n";
        cards_sprites.push_back(create_spite(
            ren,
            "art/card_back.bmp",
            (250 + i * 104) - (624 * y),
            250 + y * 86,
            52,
            86,
            "card" + i,
            "card",
            5
        ));
        for (int i2 = 0; i2 < size(sp_card_indexs); i2++) {
            if (random(1, sp_card_indexs[i2].second) == 1) {
                sp_card = 1;
                r       = sp_card_indexs[i2].first;
            } else {
                if (sp_card != 1) {
                    r = random(2, 7);
                }
            }
        }
        if (sp_card == 1) {
            cards_left -= 1;
            std::cout << "spc";
            std::cout << cards_left << "\n";
        }
        std::cout << ">" << r << "\n";
        card_sprites_indexs.push_back(r);
        cards_sprites_des_frames.push_back(40);
        create_button((250 + i * 104) - (624 * y), 250 + y * 86, 26, 43, "card");
        if ((250 + i * 104) - (624 * y) > 750) {
            y += 1;
        }
        std::cout << "end\n";
    }
    rest_orders();
}
void make_hearts(SDL_Renderer * ren, int health) {
    for (int i = 0; i < health; i++) {
        hearts.push_back(create_spite(
            ren,
            "art/heart.bmp",
            150 + i * (70 - (35 * floor(health / 11))),
            460,
            52 - (27 * floor(health / 11)),
            52 - (27 * floor(health / 11)),
            "heart" + i,
            "ui",
            2
        ));
    }
    rest_orders();
}
void load_level_(SDL_Renderer * ren, TTF_Font * Sans) {
    make_cards(ren, cards);
    make_hearts(ren, health);
    // load ui
    spritesnc.push_back(
        create_spite(ren, "art/bottem_ui.bmp", 500, 400, 1000, 200, "bottem_tool_bar", "ui", 2)
    );
    // dev tools
    // spritesnc.push_back(create_spite(ren,"art/heart.bmp", 150, 90, 50, 60, "damge_button",
    // "ui", 2)); create_button(150,90,25,30, "ui");
    // spritesnc.push_back(create_spite(ren,"art/heart.bmp", 250, 90, 50, 60, "heal_button",
    // "ui", 2)); create_button(250,90,25,30 , "ui");
    // spritesnc.push_back(create_spite(ren,"art/hand.bmp", 350, 90, 50, 60, "win_button", "ui",
    // 2)); create_button(350,90,25,30, "ui");
    // spritesnc.push_back(create_spite(ren,"art/hand.bmp", 450, 90, 50, 60, "lose_button",
    // "ui", 2)); create_button(450,90,25,30, "ui");
    random_card = create_spite(ren, "art/card_back.bmp", 75, 225, 100, 150, "rand_card", "ui", 2);
    win_lose_ui = create_spite(ren, "null", NULL, NULL, NULL, NULL, "", "", 1);
    // player and hand
    player = create_spite(
        ren,
        "art/player.bmp",
        cards_sprites[0].sprite_rec.x,
        cards_sprites[0].sprite_rec.y,
        50,
        60,
        "player",
        "player",
        2
    );
    hand = create_spite(ren, "art/hand.bmp", 0, 0, 50, 60, "player_hand", "ui", 1);
    texts.push_back(create_text(
        ren,
        (std::to_string(health_max) + "/" + std::to_string(health)).c_str(),
        11,
        { 0, 0, 0 },
        Sans,
        650,
        370
    ));
    // Recache
    rest_orders();
}
std::string get_sprite_of_item(std::string str) {
    for (int i = 0; i < size(items); i++) {
        if (items[i][0] == str) {
            return items[i][1];
        }
    }
}
void tick_hand() {
    SDL_GetMouseState(&mouseX, &mouseY);
    hand.sprite_rec.x = mouseX;
    hand.sprite_rec.y = mouseY;
    std::cout << "moved_mouse\n";
}
void erase_level() {
    for (int i = 0; i < size(hearts); i++) {
        destory_sprite(&hearts[i]);
    }
    for (int i = 0; i < size(cards_sprites); i++) {
        destory_sprite(&cards_sprites[i]);
    }
    for (int i = 0; i < size(spritesnc); i++) {
        destory_sprite(&spritesnc[i]);
    }
    for (int i = 0; i < size(texts); i++) {
        destory_text(&texts[i]);
    }
    hearts.clear();
    cards_sprites.clear();
    spritesnc.clear();
    texts.clear();
    destory_sprite(&player);
    destory_sprite(&random_card);
    buttons.clear();
    rest_orders();
}
void load_victory_ui(SDL_Renderer * ren, TTF_Font * Sans) {
    spritesnc.push_back(
        create_spite(ren, "art/retry_button.bmp", 500, 400, 100, 80, "retry_button", "ui", 2)
    );
    create_button(500, 400, 50, 40, "ui");
    if (win_lose_ui.name == "victory_sprite") {
        spritesnc.push_back(
            create_spite(ren, "art/next_button.bmp", 650, 400, 100, 80, "next_button", "ui", 2)
        );
        create_button(650, 400, 50, 40, "ui");
        texts.push_back(create_text(ren, "treasure:", 12, { 0, 0, 0 }, Sans, 150, 500));
        for (int i = 0; i < size(collected_items); i++) {
            spritesnc.push_back(create_spite(
                ren,
                get_sprite_of_item(collected_items[i]).c_str(),
                80,
                500,
                50,
                80,
                "item" + i,
                "ui",
                2
            ));
            texts.push_back(create_text(
                ren,
                (std::to_string(collected_items_amount[i]) + "x").c_str(),
                12,
                { 0, 0, 0 },
                Sans,
                200,
                500
            ));
        }
    }
    rest_orders();
}
void load_level_reset(SDL_Renderer * ren, TTF_Font * Sans) {
    erase_level();
    destory_sprite(&win_lose_ui);
    destory_sprite(&hand);
    card_sprites_indexs      = {};
    cards_sprites_des_frames = {};
    load_level               = true;
    won                      = -1;
    cards_left               = cards;
    health                   = health_max;
    max_health               = health_max;
    heart                    = 0;
    moved_this_frame         = false;
    random_card_index        = -1;
    load_level_(ren, Sans);
}
void tick_victroy_ui(SDL_Renderer * ren, TTF_Font * Sans) {
    if (buttons[0].state == 1) {
        load_level_reset(ren, Sans);
        return;
    } else if (buttons[0].state == 2) {
        change_sprite_width_height(110, 90, &spritesnc[0]);
    } else {
        change_sprite_width_height(100, 80, &spritesnc[0]);
    }
    if (win_lose_ui.name == "victory_sprite") {
        if (buttons[1].state == 1) {
            cards += 6;
            load_level_reset(ren, Sans);
            return;
        } else if (buttons[1].state == 2) {
            change_sprite_width_height(110, 90, &spritesnc[1]);
        } else {
            change_sprite_width_height(100, 80, &spritesnc[1]);
        }
        if (texts[0].text_rec.y >= 50) {
            texts[0].text_rec.y -= round((abs(texts[0].text_rec.y - 50) / 10) + 0.5);
        }
        for (int i = 2; i < size(spritesnc); i++) {
            if (spritesnc[i].sprite_rec.y >= 200 + ((i - 2) * 100)) {
                spritesnc[i].sprite_rec.y -=
                    round((abs(spritesnc[i].sprite_rec.y - (200 + ((i - 2) * 100))) / 10) + 0.5);
                texts[i - 1].text_rec.y -=
                    round((abs(texts[i - 1].text_rec.y - (200 + ((i - 2) * 100))) / 10) + 0.5);
            }
        }
    }
}
void render_ui(SDL_Renderer * ren) {
    if (health > -1) {
        for (int i = 0; i < max_health - health; i++) {
            destory_sprite(&hearts[heart]);
            max_health -= 1;
            shake_frame = 20;
            heart += 1;
            std::cout << "erased heart\n";
        }
    } else {
        health = 0;
    }
    if (health == 0) {
        health = 1;
        won    = -2;
    }
    // if (buttons[size(buttons)-3].state == 1){
    //     for(int i=0; i < health; i++){
    //              hearts.erase(hearts.begin());

    //              std::cout << "erased heart2\n";
    //     }
    //      max_health = health_max;
    //      health = health_max;
    //      make_hearts(ren, health);

    // }
}
void load_defeat_victory_ui(SDL_Renderer * ren, TTF_Font * Sans) {
    if (won == 1) {
        destory_sprite(&win_lose_ui);
        win_lose_ui =
            create_spite(ren, "art/victory.bmp", 500, 500, 400, 150, "victory_sprite", "ui", 1);
        load_level = false;
        won        = -1;
    }
    if (won == -2) {
        destory_sprite(&win_lose_ui);
        win_lose_ui =
            create_spite(ren, "art/defeat.bmp", 500, 500, 400, 150, "defeat_sprite", "ui", 1);
        load_level = false;
        won        = -1;
    }
    if (win_lose_ui.sprite_rec.y > 100) {
        win_lose_ui.sprite_rec.y -= round((abs(win_lose_ui.sprite_rec.y - 100) / 10) + 0.5);
    } else {
        if (win_lose_ui.sprite_rec.y <= 100 && (win_lose_ui.name == "defeat_sprite")
            || (win_lose_ui.name == "victory_sprite"))
        {
            std::cout << "won:" << won << "\n";
            if (won != 3) {
                erase_level();
                load_victory_ui(ren, Sans);
            }
            won = 3;
            tick_victroy_ui(ren, Sans);
        }
    }
}
void tick_cards(SDL_Renderer * ren) {
    for (int i = 0; i < size(cards_sprites); i++) {
        if (cards_sprites[i].tag == "destoryed_card") {
            if (cards_sprites_des_frames[i] > 20) {
                if (random(1, round((cards_sprites_des_frames[i] / 5) + 0.5)) == 1) {
                    change_sprite_width_height(
                        cards_sprites[i].sprite_rec.w + random(0, 5),
                        cards_sprites[i].sprite_rec.h,
                        &cards_sprites[i]
                    );
                } else {
                    change_sprite_width_height(
                        cards_sprites[i].sprite_rec.w - random(0, 5),
                        cards_sprites[i].sprite_rec.h,
                        &cards_sprites[i]
                    );
                }
            } else {
                if (cards_sprites_des_frames[i] != 0) {
                    if (card_sprites_indexs[i] == 1 || card_sprites_indexs[i] == 0) {
                        change_sprite("art/card_boom_boom_bad.bmp", &cards_sprites[i], ren);
                        change_sprite_width_height(104, 164, &cards_sprites[i]);
                        shake_frame = 10;
                    } else {
                        change_sprite("art/card_boom_boom.bmp", &cards_sprites[i], ren);
                        change_sprite_width_height(104, 164, &cards_sprites[i]);
                        shake_frame = 10;
                    }
                } else {
                    destory_sprite(&cards_sprites[i]);
                    card_sprites_indexs[i] = -1;
                }
            }
            cards_sprites_des_frames[i] -= 1;
        }
        if (buttons[i].state == 1 && (load_level && cards_sprites[i].tag == "card")) {
            player.sprite_rec.x = cards_sprites[i].sprite_rec.x;
            player.sprite_rec.y = cards_sprites[i].sprite_rec.y;
            moved_this_frame    = true;
            moves += 1;
        }
        if (buttons[i].state == 2 && (load_level && cards_sprites[i].tag == "card")) {
            std::cout << "hover" << i << "\n";
            change_sprite_width_height(62, 92, &cards_sprites[i]);
        }
        if (!buttons[i].state && load_level) {
            change_sprite_width_height(52, 82, &cards_sprites[i]);
        }
        if (cards_sprites[i].tag == "card"
            && ((player.sprite_rec.x + 208) > cards_sprites[i].sprite_rec.x)
            && ((player.sprite_rec.x - 208) < cards_sprites[i].sprite_rec.x)
            && ((player.sprite_rec.y + 172) > cards_sprites[i].sprite_rec.y)
            && ((player.sprite_rec.y - 172) < cards_sprites[i].sprite_rec.y)
            && !(
                cards_sprites[i].sprite_rec.x == player.sprite_rec.x
                && cards_sprites[i].sprite_rec.y == player.sprite_rec.y
            ))
        {
            change_sprite(
                ("art/card_front" + std::to_string(card_sprites_indexs[i]) + ".bmp").c_str(),
                &cards_sprites[i],
                ren
            );
        } else {
            if (cards_sprites[i].tag == "card") {
                change_sprite("art/card_back.bmp", &cards_sprites[i], ren);
            }
        }
        if (random_card_index != -1 && moved_this_frame
            && cards_sprites[i].sprite_rec.x == player.sprite_rec.x
            && cards_sprites[i].sprite_rec.y == player.sprite_rec.y
            && cards_sprites[i].tag == "card" && random_card_index != card_sprites_indexs[i]
            && card_sprites_indexs[i] != 1 && card_sprites_indexs[i] != 0)
        {
            std::cout << "ouch";
            health -= 1;
        }
        if (cards_sprites[i].sprite_rec.x == player.sprite_rec.x
            && cards_sprites[i].sprite_rec.y == player.sprite_rec.y && moved_this_frame
            && cards_sprites[i].tag == "card")
        {
            if (card_sprites_indexs[i] == 0) {
                cards_sprites[i].tag = "destoryed_card";
                change_sprite(
                    ("art/card_front" + std::to_string(card_sprites_indexs[i]) + ".bmp").c_str(),
                    &cards_sprites[i],
                    ren
                );
                health -= 3;
            }
            if (card_sprites_indexs[i] == 1) {
                cards_sprites[i].tag = "destoryed_card";
                change_sprite(
                    ("art/card_front" + std::to_string(card_sprites_indexs[i]) + ".bmp").c_str(),
                    &cards_sprites[i],
                    ren
                );
                health -= 5;
            }
        }
        if (card_sprites_indexs[i] == random_card_index
            && cards_sprites[i].sprite_rec.x == player.sprite_rec.x
            && cards_sprites[i].sprite_rec.y == player.sprite_rec.y && moved_this_frame
            && cards_sprites[i].tag == "card")
        {
            cards_sprites[i].tag = "destoryed_card";
            change_sprite(
                ("art/card_front" + std::to_string(card_sprites_indexs[i]) + ".bmp").c_str(),
                &cards_sprites[i],
                ren
            );
            cards_left -= 1;
        } else {
        }
    }
    if (cards_left == 0 && win_lose_ui.tag != "ui") {
        won = 1;
    }
}
void handle_cam_move(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_LEFT && load_level) {
        new_cenx -= 5;
    }
    if (event.key.keysym.sym == SDLK_RIGHT && load_level) {
        new_cenx += 5;
    }
    if (event.key.keysym.sym == SDLK_UP && load_level) {
        new_ceny -= 5;
    }
    if (event.key.keysym.sym == SDLK_DOWN && load_level) {
        new_ceny += 5;
    }
}
void handle_sakes() {
    if (shake_frame > 0) {
        if (rand() % 2 == 1) {
            camx += rand() % 10;
        } else {
            camx -= rand() % 10;
        }
        if (rand() % 2 == 1) {
            camy += rand() % 10;
        } else {
            camy -= rand() % 10;
        }
        shake_frame -= 1;
    }
    if (shake_frame == 0) {
        camx = new_cenx;
        camy = new_ceny;
    }
}
void handle_card_after_move(SDL_Renderer * ren) {
    if (moved_this_frame) {
        int r = -1;
        if (won == -1) {
            while (r == -1 || r == 0 || r == 1) {
                r = card_sprites_indexs[random(0, size(card_sprites_indexs) - 1)];
            }

            random_card_index = r;
            if (random_card.name == "rand_card") {
                change_sprite(
                    ("art/card_front" + std::to_string(r) + ".bmp").c_str(),
                    &random_card,
                    ren
                );
            }
        }
    }
}
int main(int argc, char ** argv) {

    // rest random number gem
    srand(time(0));
    // get window and renderer
    SDL_Window * window = open_window();
    SDL_Renderer * ren =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::cout << "Failed to make renderer\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        std::terminate();
    }
    SDL_RenderClear(ren);
    SDL_Surface * win_suf = get_suf(window);
    // FreeConsole();
    //  init text engin
    if (TTF_Init() < 0) {
        std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << "\n";
    }
    TTF_Font * Sans = TTF_OpenFont("fonts/static/OpenSans-Medium.ttf", 24);
    load_level_(ren, Sans);

    SDL_Event event;
    while (keep_window_open) {
        // handle cam saking
        handle_sakes();
        SDL_ShowCursor(SDL_DISABLE);
        // get event
        SDL_PollEvent(&event);
        handle_cam_move(event);
        // clear
        SDL_RenderClear(ren);
        // set back ground
        SDL_SetRenderDrawColor(ren, 170, 100, 50, 255);
        tick_hand();
        handle_buttons(event);
        load_defeat_victory_ui(ren, Sans);
        // handle_button_presses(ren);
        if (load_level) {
            std::cout << "level loaded\n";
            // do stuff
            tick_cards(ren);
            // render_ui ATFTER evry thing else
            change_text(
                ren,
                (std::to_string(health_max) + "/" + std::to_string(health)).c_str(),
                11,
                Sans,
                { 0, 0, 0 },
                &texts[0]
            );
            render_ui(ren);
            handle_card_after_move(ren);
            moved_this_frame = false;
        }
        std::cout << "mouse pos:" << mouseX << ", " << mouseY << "\n";
        render_all_sprites(ren);
        // show
        SDL_RenderPresent(ren);

        check_if_user_exit_then_exit(event);
        std::cout << "__\n";
    }
    std::cout << "\nexit";
    SDL_Quit();
    return 0;
}
