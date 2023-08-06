// #include <curses.h>
// #include <ncurses.h>

// #include <cstddef>
// #include <cstdio>
// #include <functional>
// #include <iostream>
// #include <memory>
// #include <stack>
// #include <string>
// #include <vector>

// #include "nlohmann/json.hpp"




// std::string jsonString = R"(
//         {
//             "title":"main menu",
//             "submenus":[
//                 {
//                     "title":"menu 0",
//                     "options":[
//                         {
//                             "title":"option 0.0",
//                             "cb":"optioncb_0_0"
//                         },
//                         {
//                             "title":"option 0.1",
//                             "cb":"optioncb_0_1"
//                         }
//                     ],
//                     "submenus":[
//                         {
//                             "title":"menu 0.1",
//                             "options":[
//                                 {
//                                     "title":"option 0.1.1",
//                                     "cb":"optioncb_0_1_1"
//                                 }
//                             ]
//                         }
//                     ]
//                 },
//                 {
//                     "title":"menu 1",
//                     "options":[
//                         {
//                             "title":"option 1.1",
//                             "cb":"optioncb_1_1"
//                         }
//                     ]
//                 }
//             ],
//             "options":[
//                 {
//                     "title":"option 0",
//                     "cb":"optioncb_0"
//                 },
//                 {
//                     "title":"option 1",
//                     "cb":"optioncb_1"
//                 }
//             ]
//         }
//      )";

// static void option_0(std::string &notify) { notify = "option 0 selected"; }

// static void option_1(std::string &notify) { notify = "option 1 selected"; }

// static void option_0_0(std::string &notify) { notify = "option 0_0 selected"; }

// static void option_0_1(std::string &notify) { notify = "option 0_1 selected"; }

// static void option_0_1_1(std::string &notify) {
//   notify = "option 0_1_1 selected";
// }

// static void option_1_1(std::string &notify) { notify = "option 1_1 selected"; }

// struct MenuItem {
//   std::string title;
//   std::vector<std::pair<std::string, std::shared_ptr<MenuItem>>> submenus;
//   std::vector<std::pair<std::string, std::function<void(std::string &)>>>
//       options;
// };
// struct MenuContext {
//   std::shared_ptr<MenuItem> menu;
//   int selectedIdx;
//   std::string notifyMsg;
// };
// class MultiLevelMenu {
//  public:
//   MultiLevelMenu() : quit_(false) {}

//   ~MultiLevelMenu() = default;

//   void load_menu(const nlohmann::json &j) { rootMenu_ = jsonToMenuTree(j);}

//   void load_callbackmap(
//       std::map<std::string, std::function<void(std::string &)>> &callbackMap) {
//     callbackMap_ = callbackMap;
//   }

//   void run(void) {
//     init();

//     push_menu(rootMenu_);
//     while (!quit_) {
//       renderMenu();
//       handleInput();
//     }
//     end();
//   }

//  private:
//   void init(void) {
//     initscr();             // 初始化ncurses
//     noecho();              // 关闭回显
//     keypad(stdscr, true);  // 启用功能键（F1，F2等）以及箭头键
//     curs_set(0);           // 隐藏光标
//     start_color();         // 启用颜色

//     init_pair(1, COLOR_WHITE, COLOR_BLACK);
//     init_pair(2, COLOR_RED, COLOR_BLACK);
//   }

//   void end(void) {
//     endwin();  // 结束ncurses
//   }

//   void renderMenu() {
//     std::shared_ptr<MenuItem> menu = menuStack_.top().menu;
//     int &selectedIdx = menuStack_.top().selectedIdx;
//     std::string &notifyMsg = menuStack_.top().notifyMsg;

//     int optionCount = menu->options.size();
//     int submenuCount = menu->submenus.size();

//     clear();
//     mvprintw(1, 0, menu->title.c_str());
//     if (!notifyMsg.empty()) {
//       init_pair(1, COLOR_RED, COLOR_BLACK);

//       attron(COLOR_PAIR(2));
//       mvprintw(1, menu->title.size() + 4, nofiyPrefix_.c_str());
//       mvprintw(1, menu->title.size() + nofiyPrefix_.size() + 4,
//                notifyMsg.c_str());
//       attroff(COLOR_PAIR(2));
//     }

//     for (int i = 0; i < optionCount; ++i) {
//       int index = i;
//       bool isSelected = (selectedIdx == index);
//       char prefix = isSelected ? '>' : ' ';
//       const char *optionText = menu->options[i].first.c_str();
//       mvprintw(3 + index, 0, "%c %s", prefix, optionText);
//     }

//     for (int i = 0; i < submenuCount; ++i) {
//       int index = i + optionCount;
//       bool isSelected = (selectedIdx == index);
//       char prefix = isSelected ? '>' : ' ';
//       const char *submenuText = menu->submenus[i].first.c_str();
//       mvprintw(3 + index, 0, "%c %s", prefix, submenuText);
//     }

//     refresh();
//   }

//   void push_menu(std::shared_ptr<MenuItem> menu) {
//     struct MenuContext menuContext;
//     menuContext.menu = menu;
//     menuContext.selectedIdx = 0;
//     menuContext.notifyMsg.clear();
//     menuStack_.push(menuContext);
//   }

//   void handleInput() {
//     int ch = getch();

//     std::shared_ptr<MenuItem> menu = menuStack_.top().menu;
//     int &selectedIdx = menuStack_.top().selectedIdx;
//     std::string &notifyMsg = menuStack_.top().notifyMsg;

//     int optionCount = menu->options.size();
//     int submenuCount = menu->submenus.size();

//     switch (ch) {
//       case KEY_UP:
//       case 'k':
//         selectedIdx = (selectedIdx - 1 + (optionCount + submenuCount)) %
//                       (optionCount + submenuCount);
//         break;
//       case KEY_DOWN:
//       case 'j':
//         selectedIdx = (selectedIdx + 1 + (optionCount + submenuCount)) %
//                       (optionCount + submenuCount);
//         break;
//       case 10:  // Enter键
//       case KEY_RIGHT:
//       case 'l':
//         if (selectedIdx < optionCount) {
//           if (menu->options[selectedIdx].second) {
//             std::function<void(std::string &)> cb =
//                 menu->options[selectedIdx].second;
//             cb(notifyMsg);
//           }
//         } else {
//           push_menu(menu->submenus[selectedIdx - optionCount].second);
//         }

//         break;
//       case KEY_LEFT:
//       case 'h':
//         if (menuStack_.size() > 1) {
//           menuStack_.pop();
//         }
//         break;

//       case 27:  // ESC键
//         quit_ = true;
//         break;
//     }
//   }

//   std::shared_ptr<MenuItem> jsonToMenuTree(const nlohmann::json &j) {
//     std::shared_ptr<MenuItem> menu = std::make_shared<MenuItem>();

//     if (j.contains("title")) {
//       menu->title = j["title"].get<std::string>();
//     }

//     if (j.contains("submenus")) {
//       for (auto &iter : j["submenus"].items()) {
//         menu->submenus.push_back(
//             std::make_pair(iter.value()["title"].get<std::string>(),
//                            jsonToMenuTree(iter.value())));
//       }
//     }

//     if (j.contains("options")) {
//       for (auto &iter : j["options"].items()) {
//         auto cb_name = iter.value()["cb"].get<std::string>();
//         if (callbackMap_.count(cb_name)) {
//           menu->options.push_back(std::make_pair(
//               iter.value()["title"].get<std::string>(), callbackMap_[cb_name]));
//         } else {
//           std::cout << "Callback " << iter.value()["cb"].get<std::string>()
//                     << " function not found!" << std::endl;
//           break;
//         }
//       }
//     }

//     return menu;
//   }

//   bool quit_;
//   std::shared_ptr<MenuItem> rootMenu_;
//   std::stack<struct MenuContext> menuStack_;
//   std::map<std::string, std::function<void(std::string &)>> callbackMap_;
//   const std::string nofiyPrefix_ = "notify:";
// };

// int main() {
//   MultiLevelMenu menu;

//   std::map<std::string, std::function<void(std::string &)>> callbackMap = {
//       {"optioncb_0", option_0},         {"optioncb_1", option_1},
//       {"optioncb_0_0", option_0_0},     {"optioncb_0_1", option_0_1},
//       {"optioncb_0_1_1", option_0_1_1}, {"optioncb_1_1", option_1_1}};

//   menu.load_callbackmap(callbackMap);
//   menu.load_menu(nlohmann::json::parse(jsonString));
//   menu.run();

//   return 0;
// }

#include <cstddef>
#include <iostream>
#include <memory>

class Test{
public:
  void print();

  static std::unique_ptr<Test> testGen(int a);
  ~Test() = default;
   Test(int a = 0):a_(a){}
private:
 
  int a_;
};

void Test::print()
{
  std::cout<<"a = "<<a_<<std::endl;
}

std::unique_ptr<Test> Test::testGen(int a)
{
  return std::make_unique<Test>(a);
}

int main()
{
std::unique_ptr<Test> p = Test::testGen(6);
p->print();
}


