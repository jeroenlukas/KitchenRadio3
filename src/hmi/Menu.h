#ifndef KR_MENU_H
#define KR_MENU_H

#include "../system/Logger.h"

#define MAX_MENUS 3

#define MENU_SETTINGS 0
#define MENU_ALARM  1
#define MENU_LAMP 2

enum ItemType {
  ACTION_ITEM,
  CUSTOMINFO_ITEM,
  VALUE_ITEM,
  MENU_ITEM
};

class MenuItem {
  public:
    virtual const char* getName() const = 0;
    virtual ItemType getType() const = 0;
    //virtual int getValue() const = 0;
};

class InfoItem : public MenuItem {
  private:
    const char* name; // e.g. 'Weather'
    void (*onShow)() = nullptr;  // callback

  public:
    InfoItem(const char* n)
      : name(n) {}

    ItemType getType() const override {
      return CUSTOMINFO_ITEM;
    }  

    const char* getName() const override {
      return name;
    }

    void setOnShowCallback(void (*cb)()) {
      onShow = cb;
    }

    void show()
    {
      onShow();
    }
};

class ValueItem : public MenuItem {
  private:
    const char* name;
    
    int* valuePtr;
    int minVal, maxVal;
    void (*onChange)(int) = nullptr;  // callback

  public:
    ValueItem(const char* n, int* v, int minV, int maxV)
      : name(n), valuePtr(v), minVal(minV), maxVal(maxV) {}

    int increment;

    ItemType getType() const override {
      return VALUE_ITEM;
    }

    void increase() {
      if (*valuePtr < maxVal) {
        (*valuePtr) += increment;
        if (onChange) onChange(*valuePtr);
      }
    }

    void decrease() {
      if (*valuePtr > minVal) {
        (*valuePtr) -= increment;
        if (onChange) onChange(*valuePtr);
      }
    }

    int getValue() const {
      return *valuePtr;
    }

    const char* getName() const override {
      return name;
    }

    void setCallback(void (*cb)(int)) {
      onChange = cb;
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

    int getItemCount(){
      return itemCount;
    }

    int getItemIndex(){
      return selectedIndex;
    }

    void first(){
      selectedIndex = 0;
    }


    void next() {
      //selectedIndex = (selectedIndex + 1) % itemCount;
      if(selectedIndex < itemCount-1) selectedIndex++;
    }

    void prev() {
      // selectedIndex = (selectedIndex - 1 + itemCount) % itemCount;
      if(selectedIndex > 0) selectedIndex--;
    }


    
};


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
      LOGG_DEBUG("Menu: " + String(currentMenuIndex));      
    }

    void exit()
    {
      active = false;
    }

    // Delegate navigation to active menu
    void first() { currentMenu()->first(); }
    void next() { currentMenu()->next(); }
    void prev() { currentMenu()->prev(); }


};

//enum menuId_t {MENU_HOME, MENU_SYSTEM, MENU_ALARM, MENU_LAMP};

//extern menuId_t menuId;

extern Menu menuSettings;

extern MenuManager menuMgr;

extern void menu_begin();

#endif