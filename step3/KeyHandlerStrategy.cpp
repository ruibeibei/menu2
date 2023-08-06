#include "KeyHandlerStrategy.h"
#include "MultiLevelMenu.h"

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