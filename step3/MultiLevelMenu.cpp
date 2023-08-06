#include <ncurses.h>

#include <iostream>

#include "MultiLevelMenu.h"

MultiLevelMenu::MultiLevelMenu(std::unique_ptr<MenuInterface> menuIntf) : quit_(false)
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

void MultiLevelMenu::load_menu(const json &j)
{
    rootMenu_ = jsonToMenuTree(j);
}

void MultiLevelMenu::load_callbackmap(
    const std::unordered_map<std::string, std::function<void(std::string &)>> &callbackMap)
{
    callbackMap_ = callbackMap;
}

void MultiLevelMenu::run(void)
{
    init();

    push_menu(rootMenu_);
    while (!quit_) {
        renderMenu();
        handleInput();
    }
    destory();
}

void MultiLevelMenu::push_menu(std::shared_ptr<MenuItem> menu)
{
    struct MenuContext menuContext;
    menuContext.menu = menu;
    menuContext.selectedIdx = 0;
    menuContext.notifyMsg.clear();
    menuStack_.push(menuContext);
}
void MultiLevelMenu::pop_menu(void)
{
    if (menuStack_.size() > 1) {
        menuStack_.pop();
        if (!menuStack_.empty()) {
            menuStack_.top().notifyMsg.clear();
        }
    }
}

void MultiLevelMenu::init(void)
{
    menuInterface_->menuInit();
}

void MultiLevelMenu::destory(void)
{
    menuInterface_->menuDestory();
}

void MultiLevelMenu::renderMenu()
{
    menuInterface_->renderMenu(menuStack_.top());
}

void MultiLevelMenu::handleInput()
{
    int ch = getch();
    auto it = keyHandlerStrategyMap_.find(ch);
    if (it != keyHandlerStrategyMap_.end()) {
        it->second->handleKey(menuStack_.top());
    }
}

std::shared_ptr<MenuItem> MultiLevelMenu::jsonToMenuTree(const json &j)
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
