#pragma once

#include <string>

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
    void menuInit(void) override;

    void menuDestory(void) override;

    void renderMenu(struct MenuContext &menu_ctx) override;

private:
    const std::string nofiyPrefix_ = "notify:";
};
