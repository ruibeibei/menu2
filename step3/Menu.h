#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

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
