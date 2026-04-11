#ifndef KR_MENU_H
#define KR_MENU_H

#include "../system/Logger.h"

enum ItemType {
  ACTION_ITEM,
  VALUE_ITEM,
  MENU_ITEM
};

class MenuItem {
  public:
    virtual const char* getName() const = 0;
    virtual ItemType getType() const = 0;
    //virtual int getValue() const = 0;
};

class ValueItem : public MenuItem {
  private:
    const char* name;
    int value;
    int minVal, maxVal;

  public:
    ValueItem(const char* n, int v, int minV, int maxV) {
      name = n;
      value = v;
      minVal = minV;
      maxVal = maxV;
    }

    ItemType getType() const override {
      return VALUE_ITEM;
    }

    void increase() {
      if (value < maxVal) value++;
    }

    void decrease() {
      if (value > minVal) value--;
    }

    int getValue() const {
      return value;
    }

    const char* getName() const override {
      return name;
    }
};

#define MAX_ITEMS 10
class Menu : public MenuItem {
  private:
    const char* title;
    MenuItem* items[MAX_ITEMS];
    int itemCount = 0;
    int selectedIndex = 0;

  public:
    Menu(const char* t) : title(t) {}

    ItemType getType() const override {
      return MENU_ITEM;
    }

    const char* getName() const override{
      return title;
    }

    MenuItem* getSelectedItem(){
      return items[selectedIndex];
    }

    void addItem(MenuItem* item) {
      if (itemCount < MAX_ITEMS) {
        items[itemCount++] = item;
      }
    }

    void next() {
      selectedIndex = (selectedIndex + 1) % itemCount;
      //display();
    }

    void prev() {
      selectedIndex = (selectedIndex - 1 + itemCount) % itemCount;
      //display();
    }


    
};
#define MAX_MENUS 3

#define MENU_SETTINGS 0
#define MENU_ALARM  1
#define MENU_LAMP 2

class MenuManager {
  private:
    Menu* menus[MAX_MENUS];
    int menuCount = 0;
    bool active = false;
    int currentMenuIndex = MENU_SETTINGS;

  public:
    bool isActive() // Are we in a menu or in the home screen?
    {
      return active;
    }

    void addMenu(Menu* menu) {
      if (menuCount < MAX_MENUS) {
        menus[menuCount++] = menu;
      }
    }

    Menu* currentMenu() {
      return menus[currentMenuIndex];
    }

    int currentMenuID()
    {
      return currentMenuIndex;
    }

    void switchTo(int id) {
      currentMenuIndex = id;
      active = true;
      display();
    }

    void exit()
    {
      active = false;
    }

    /*void nextMenu() {
      currentMenuIndex = (currentMenuIndex + 1) % menuCount;
    }

    void prevMenu() {
      currentMenuIndex = (currentMenuIndex - 1 + menuCount) % menuCount;
    }*/

    // Delegate navigation to active menu
    void next() { currentMenu()->next(); }
    void prev() { currentMenu()->prev(); }
    //void select() { currentMenu()->select(); }

    void display() {
      Serial.print("[Menu ");
      Serial.print(currentMenuIndex);
      Serial.println("]");
      //currentMenu()->display();
    }
};

//enum menuId_t {MENU_HOME, MENU_SYSTEM, MENU_ALARM, MENU_LAMP};

//extern menuId_t menuId;

extern Menu menuSettings;

extern MenuManager menuMgr;

extern void menu_begin();

#endif