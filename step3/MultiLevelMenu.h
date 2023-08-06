#pragma once

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <stack>
#include <unordered_map>

#include "KeyHandlerStrategy.h"
#include "Menu.h"
#include "MenuInterface.h"

using json = nlohmann::json;

class MultiLevelMenu
{
public:
    explicit MultiLevelMenu(std::unique_ptr<MenuInterface> menuIntf);
    ~MultiLevelMenu() = default;

    void load_menu(const json &j);
    void load_callbackmap(
        const std::unordered_map<std::string, std::function<void(std::string &)>> &callbackMap);
    void run(void);
    void push_menu(std::shared_ptr<MenuItem> menu);
    void pop_menu(void);

private:
    void init(void);
    void destory(void);
    void renderMenu();
    void handleInput();
    std::shared_ptr<MenuItem> jsonToMenuTree(const json &j);

    bool quit_;
    std::shared_ptr<MenuItem> rootMenu_;
    std::stack<struct MenuContext> menuStack_;
    std::unique_ptr<MenuInterface> menuInterface_;
    std::unordered_map<std::string, std::function<void(std::string &)>> callbackMap_;
    std::unordered_map<int, std::shared_ptr<KeyHandlerStrategy>> keyHandlerStrategyMap_;
};
