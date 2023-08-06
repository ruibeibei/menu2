#include <curses.h>
#include <ncurses.h>

#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using json = nlohmann::json;

std::string jsonString = R"(
        {
            "title":"main menu",
            "submenus":[
                {
                    "title":"menu 0",
                    "options":[
                        {
                            "title":"option 0.0",
                            "cb":"optioncb_0_0"
                        },
                        {
                            "title":"option 0.1",
                            "cb":"optioncb_0_1"
                        }
                    ],
                    "submenus":[
                        {
                            "title":"menu 0.1",
                            "options":[
                                {
                                    "title":"option 0.1.1",
                                    "cb":"optioncb_0_1_1"
                                }
                            ]
                        }
                    ]
                },
                {
                    "title":"menu 1",
                    "options":[
                        {
                            "title":"option 1.1",
                            "cb":"optioncb_1_1"
                        }
                    ]
                }
            ],
            "options":[
                {
                    "title":"option 0",
                    "cb":"optioncb_0"
                },
                {
                    "title":"option 1",
                    "cb":"optioncb_1"
                }
            ]
        }
     )";

static void option_0(std::string &notify)
{
    notify = "option 0 selected";
}

static void option_1(std::string &notify)
{
    notify = "option 1 selected";
}

static void option_0_0(std::string &notify)
{
    notify = "option 0_0 selected";
}

static void option_0_1(std::string &notify)
{
    notify = "option 0_1 selected";
}

static void option_0_1_1(std::string &notify)
{
    notify = "option 0_1_1 selected";
}

static void option_1_1(std::string &notify)
{
    notify = "option 1_1 selected";
}

struct MenuItem {
    std::string title;
    std::vector<std::pair<std::string, std::shared_ptr<MenuItem>>> submenus;
    std::vector<std::pair<std::string, std::function<void(std::string &)>>> options;
};
struct MenuContext {
    std::shared_ptr<MenuItem> menu;
    int selectedIdx;
    std::string notifyMsg;
};

class MenuInterface
{
public:
    virtual ~MenuInterface() = default;
    virtual void menuInit(void) = 0;
    virtual void menuDestory(void) = 0;
    virtual void renderMenu(struct MenuContext &) = 0;
};

class NcursesMenuInterface : public MenuInterface
{
public:
    ~NcursesMenuInterface() override = default;
    void menuInit(void) override
    {
        initscr();            // 初始化ncurses
        noecho();             // 关闭回显
        keypad(stdscr, true); // 启用功能键（F1，F2等）以及箭头键
        curs_set(0);          // 隐藏光标
        start_color();        // 启用颜色

        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        attron(COLOR_PAIR(1));
    }

    void menuDestory(void) override
    {
        attroff(COLOR_PAIR(1));
        endwin(); // 结束ncurses
    }

    void renderMenu(struct MenuContext &menu_ctx) override
    {
        std::shared_ptr<MenuItem> menu = menu_ctx.menu;
        int &selectedIdx = menu_ctx.selectedIdx;
        std::string &notifyMsg = menu_ctx.notifyMsg;

        int optionCount = menu->options.size();
        int submenuCount = menu->submenus.size();

        clear();
        mvprintw(1, 0, menu->title.c_str());
        if (!notifyMsg.empty()) {
            attron(COLOR_PAIR(2));
            mvprintw(1, menu->title.size() + 4, nofiyPrefix_.c_str());
            mvprintw(1, menu->title.size() + nofiyPrefix_.size() + 4, notifyMsg.c_str());
            attroff(COLOR_PAIR(2));
        }

        for (int i = 0; i < optionCount; ++i) {
            int index = i;
            bool isSelected = (selectedIdx == index);
            char prefix = isSelected ? '>' : ' ';
            const char *optionText = menu->options[i].first.c_str();
            mvprintw(3 + index, 0, "%c %s", prefix, optionText);
        }

        for (int i = 0; i < submenuCount; ++i) {
            int index = i + optionCount;
            bool isSelected = (selectedIdx == index);
            char prefix = isSelected ? '>' : ' ';
            const char *submenuText = menu->submenus[i].first.c_str();
            mvprintw(3 + index, 0, "%c %s", prefix, submenuText);
        }

        refresh();
    }

private:
    const std::string nofiyPrefix_ = "notify:";
};

class MultiLevelMenu;
class KeyHandlerStrategy
{
public:
    explicit KeyHandlerStrategy(MultiLevelMenu *menu) : menu_(menu)
    {}
    virtual ~KeyHandlerStrategy() = default;
    virtual void handleKey(MenuContext &menuCtx) = 0;

protected:
    MultiLevelMenu *menu_;
};

class KeyHandlerPrevItem : public KeyHandlerStrategy
{
public:
    KeyHandlerPrevItem(MultiLevelMenu *menu) : KeyHandlerStrategy(menu)
    {}
    void handleKey(MenuContext &menuCtx) override;
};
class KeyHandlerNextItem : public KeyHandlerStrategy
{
public:
    KeyHandlerNextItem(MultiLevelMenu *menu) : KeyHandlerStrategy(menu)
    {}
    void handleKey(MenuContext &menuCtx) override;
};
class KeyHandlerNextLevel : public KeyHandlerStrategy
{
public:
    KeyHandlerNextLevel(MultiLevelMenu *menu) : KeyHandlerStrategy(menu)
    {}
    void handleKey(MenuContext &menuCtx) override;
};
class KeyHandlerPrevLevel : public KeyHandlerStrategy
{
public:
    KeyHandlerPrevLevel(MultiLevelMenu *menu) : KeyHandlerStrategy(menu)
    {}
    void handleKey(MenuContext &menuCtx) override;
};
class MultiLevelMenu
{
public:
    explicit MultiLevelMenu(std::unique_ptr<MenuInterface> menuIntf) : quit_(false)
    {
        std::shared_ptr<KeyHandlerStrategy> nextItemHandler =
            std::make_shared<KeyHandlerNextItem>(this);
        std::shared_ptr<KeyHandlerStrategy> prevItemHandler =
            std::make_shared<KeyHandlerPrevItem>(this);
        std::shared_ptr<KeyHandlerStrategy> nextLevelHandler =
            std::make_shared<KeyHandlerNextLevel>(this);
        std::shared_ptr<KeyHandlerStrategy> PrevLevelHandler =
            std::make_shared<KeyHandlerPrevLevel>(this);

        keyHandlerStrategyMap_.insert(std::make_pair(KEY_DOWN, nextItemHandler));
        keyHandlerStrategyMap_.insert(std::make_pair('j', nextItemHandler));

        keyHandlerStrategyMap_.insert(std::make_pair(KEY_UP, prevItemHandler));
        keyHandlerStrategyMap_.insert(std::make_pair('k', prevItemHandler));

        keyHandlerStrategyMap_.insert(std::make_pair(KEY_RIGHT, nextLevelHandler));
        keyHandlerStrategyMap_.insert(std::make_pair('l', nextLevelHandler));
        keyHandlerStrategyMap_.insert(std::make_pair('\n', nextLevelHandler));

        keyHandlerStrategyMap_.insert(std::make_pair(KEY_LEFT, PrevLevelHandler));
        keyHandlerStrategyMap_.insert(std::make_pair('h', PrevLevelHandler));

        menuInterface_ = std::move(menuIntf);
    }

    ~MultiLevelMenu() = default;

    void load_menu(const json &j)
    {
        rootMenu_ = jsonToMenuTree(j);
    }

    void load_callbackmap(
        const std::unordered_map<std::string, std::function<void(std::string &)>> &callbackMap)
    {
        callbackMap_ = callbackMap;
    }

    void run(void)
    {
        init();

        push_menu(rootMenu_);
        while (!quit_) {
            renderMenu();
            handleInput();
        }
        destory();
    }

    void push_menu(std::shared_ptr<MenuItem> menu)
    {
        struct MenuContext menuContext;
        menuContext.menu = menu;
        menuContext.selectedIdx = 0;
        menuContext.notifyMsg.clear();
        menuStack_.push(menuContext);
    }
    void pop_menu(void)
    {
        if (menuStack_.size() > 1) {
            menuStack_.pop();
            if (!menuStack_.empty()) {
                menuStack_.top().notifyMsg.clear();
            }
        }
    }

private:
    void init(void)
    {
        menuInterface_->menuInit();
    }

    void destory(void)
    {
        menuInterface_->menuDestory();
    }

    void renderMenu()
    {
        menuInterface_->renderMenu(menuStack_.top());
    }

    void handleInput()
    {
        int ch = getch();
        auto it = keyHandlerStrategyMap_.find(ch);
        if (it != keyHandlerStrategyMap_.end()) {
            it->second->handleKey(menuStack_.top());
        }
    }

    std::shared_ptr<MenuItem> jsonToMenuTree(const json &j)
    {
        auto menu = std::make_shared<MenuItem>();

        if (j.contains("title")) {
            menu->title = j["title"].get<std::string>();
        }

        if (j.contains("submenus")) {
            for (auto &iter : j["submenus"].items()) {
                menu->submenus.push_back(std::make_pair(iter.value()["title"].get<std::string>(),
                                                        jsonToMenuTree(iter.value())));
            }
        }

        if (j.contains("options")) {
            for (auto &iter : j["options"].items()) {
                auto cb_name = iter.value()["cb"].get<std::string>();
                if (callbackMap_.count(cb_name)) {
                    menu->options.push_back(std::make_pair(iter.value()["title"].get<std::string>(),
                                                           callbackMap_[cb_name]));
                } else {
                    std::cout << "Callback " << iter.value()["cb"].get<std::string>()
                              << " function not found!" << std::endl;
                    break;
                }
            }
        }

        return menu;
    }

    bool quit_;
    std::shared_ptr<MenuItem> rootMenu_;
    std::stack<struct MenuContext> menuStack_;
    std::unique_ptr<MenuInterface> menuInterface_;
    std::unordered_map<std::string, std::function<void(std::string &)>> callbackMap_;
    std::unordered_map<int, std::shared_ptr<KeyHandlerStrategy>> keyHandlerStrategyMap_;
};

void KeyHandlerPrevItem::handleKey(MenuContext &menuCtx)
{
    auto menu = menuCtx.menu;
    int &selectedIdx = menuCtx.selectedIdx;
    int itemCount = menu->options.size() + menu->submenus.size();

    selectedIdx = (selectedIdx - 1 + itemCount) % itemCount;
}

void KeyHandlerNextItem::handleKey(MenuContext &menuCtx)
{
    auto menu = menuCtx.menu;
    int &selectedIdx = menuCtx.selectedIdx;
    int itemCount = menu->options.size() + menu->submenus.size();

    selectedIdx = (selectedIdx + 1 + itemCount) % itemCount;
}

void KeyHandlerNextLevel::handleKey(MenuContext &menuCtx)
{
    auto menu = menuCtx.menu;
    int &selectedIdx = menuCtx.selectedIdx;
    int optionsCount = menu->options.size();

    if (selectedIdx < optionsCount) {
        if (menu->options[selectedIdx].second) {
            menu->options[selectedIdx].second(menuCtx.notifyMsg);
        }
    } else {
        menu_->push_menu(menu->submenus[selectedIdx - menu->options.size()].second);
    }
}

void KeyHandlerPrevLevel::handleKey(MenuContext &menuCtx)
{
    (void)menuCtx;
    menu_->pop_menu();
}

int main()
{
    MultiLevelMenu menu(std::make_unique<NcursesMenuInterface>());
    std::unordered_map<std::string, std::function<void(std::string &)>> callbackMap = {
        {"optioncb_0", option_0},         {"optioncb_1", option_1},
        {"optioncb_0_0", option_0_0},     {"optioncb_0_1", option_0_1},
        {"optioncb_0_1_1", option_0_1_1}, {"optioncb_1_1", option_1_1}};

    menu.load_callbackmap(callbackMap);
    menu.load_menu(json::parse(jsonString));
    menu.run();

    return 0;
}
