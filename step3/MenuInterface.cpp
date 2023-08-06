#include <ncurses.h>

#include "Menu.h"
#include "MenuInterface.h"

void NcursesMenuInterface::menuInit(void)
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

void NcursesMenuInterface::menuDestory(void)
{
    attroff(COLOR_PAIR(1));
    endwin(); // 结束ncurses
}

void NcursesMenuInterface::renderMenu(struct MenuContext &menu_ctx)
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
