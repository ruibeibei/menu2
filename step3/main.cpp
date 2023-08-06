#include <string>

#include "MultiLevelMenu.h"
#include "nlohmann/json.hpp"

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
