#pragma once
#include "KeyHandlerStrategy.h"
#include "Menu.h"

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