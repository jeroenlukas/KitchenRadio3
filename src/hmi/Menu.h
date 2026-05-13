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
  INT_ITEM,
  FLOAT_ITEM,
  OPTION_ITEM,
  BOOL_ITEM,
  MENU_ITEM
};


// Base class for menu items
class MenuItem {
  public:
    virtual const char* getName() const = 0;
    virtual ItemType getType() const = 0;    
    virtual void increase() = 0;
    virtual void decrease() = 0;
};

// =========================== ITEMS ===========================

// This is an info page item.
// It should be used with the onShow callback, where a custom display is drawn.
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

    void increase() override {} // Ignore
    void decrease() override {} // Ignore

    void show()
    {
      onShow();
    }
};

// Menu item for an integer value
class IntItem : public MenuItem {
  private:
    const char* name;
    
    int* valuePtr;
    int minVal, maxVal;
    void (*onChange)(int) = nullptr;  // callback

  public:
    IntItem(const char* n, int* v, int minV, int maxV)
      : name(n), valuePtr(v), minVal(minV), maxVal(maxV) {}

    int increment = 1;
    bool wraparound = false;

    ItemType getType() const override {
      return INT_ITEM;
    }

    void increase() override {
      if (*valuePtr < maxVal) {
        (*valuePtr) += increment;
        if (onChange) onChange(*valuePtr);
      }
      else if(wraparound)
      {
        (*valuePtr) = minVal;
        if (onChange) onChange(*valuePtr);
      }
    }

    void decrease() override {
      if (*valuePtr > minVal) {
        (*valuePtr) -= increment;
        if (onChange) onChange(*valuePtr);
      }
      else if(wraparound)
      {
        (*valuePtr) = maxVal;
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

// Menu item for a float value
class FloatItem : public MenuItem {
  private:
    const char* name;
    
    float* valuePtr;
    float minVal, maxVal;
    void (*onChange)(float) = nullptr;  // callback

  public:
    FloatItem(const char* n, float* v, float minV, float maxV)
      : name(n), valuePtr(v), minVal(minV), maxVal(maxV) {}

    // Item customization
    float increment = 0.1;
    bool wraparound = false; 
    int decimals = 2;

    ItemType getType() const override {
      return FLOAT_ITEM;
    }

    void increase() override {
      if (*valuePtr < maxVal) 
      {               
        (*valuePtr) += increment;
        if((*valuePtr > maxVal-0.00001) && wraparound) (*valuePtr) = minVal;

        if (onChange) onChange(*valuePtr);
      }
    }

    void decrease() override {
      if (*valuePtr > minVal) 
      {        
        if(*valuePtr > increment) (*valuePtr) -= increment;
        else *valuePtr = minVal;
        
        if((*valuePtr < minVal+0.00001) && wraparound) (*valuePtr) = maxVal; 

        if (onChange) onChange(*valuePtr);
      }
    }

    float getValue() const {
      return *valuePtr;
    }

    const char* getName() const override {
      return name;
    }

    void setCallback(void (*cb)(float)) {
      onChange = cb;
    }
};

// Boolean menu item to switch between true and false. 
// Custom labels for true/false can be set, defaults are "On" and "Off"
class BoolItem : public MenuItem {
  private:
    const char* name;
    bool* valuePtr;

    const char* trueLabel;
    const char* falseLabel;

    void (*onChange)(bool) = nullptr;

  public:
    BoolItem(const char* n, bool* v,
                  const char* tLabel = "On",
                  const char* fLabel = "Off")
      : name(n), valuePtr(v),
        trueLabel(tLabel), falseLabel(fLabel) {}

    ItemType getType() const override {
      return BOOL_ITEM;
    }

    bool wraparound = false;

    void setCallback(void (*cb)(bool)) {
      onChange = cb;
    }

    bool getValue() const {
      return *valuePtr;
    }

    const char* getValueString()
    {
      return *valuePtr ? trueLabel : falseLabel;
    }

    void toggle() {
      *valuePtr = !(*valuePtr);
      if (onChange) onChange(*valuePtr);
    }
    
    void increase() override 
    { 
      if(wraparound || !(*valuePtr))
        toggle(); 
    }
    
    void decrease() override 
    { 
      if(wraparound || (*valuePtr))
        toggle(); 
    }

    void setValue(bool v) {
      if (*valuePtr != v) {
        *valuePtr = v;
        if (onChange) onChange(*valuePtr);
      }
    }

    const char* getName() const override {
      return name;
    }
};

class OptionItem : public MenuItem {
  private:
    const char* name;
    int* valuePtr;                 // pointer to enum (stored as int)
    const char** labels;           // array of names
    int optionCount;

    void (*onChange)(int) = nullptr;

  public:
    OptionItem(const char* n, int* v, const char** l, int count)
      : name(n), valuePtr(v), labels(l), optionCount(count) {}

    bool wraparound = false;

    ItemType getType() const override {
      return OPTION_ITEM;
    }

    void setCallback(void (*cb)(int)) {
      onChange = cb;
    }

    int getValue() const {
      return *valuePtr;
    }

    const char* getValueString()
    {
      return labels[*valuePtr];
    }

    void increase() override {
      Serial.println("Valueptr: " + String(*valuePtr) + " optionCount: " + String(optionCount) + " wraparound: " + String(wraparound));
      if(((*valuePtr) < optionCount-1) || wraparound)
      {
        *valuePtr = (*valuePtr + 1) % optionCount;
        if (onChange) onChange(*valuePtr);
      }
    }

    void decrease() override {
      Serial.println("Valueptr: " + String(*valuePtr) + " optionCount: " + String(optionCount) + " wraparound: " + String(wraparound));
      if(*valuePtr > 0)
      {
        *valuePtr = (*valuePtr - 1) % optionCount;
        if (onChange) onChange(*valuePtr);
      }
      else if (wraparound)
      {
        *valuePtr =  optionCount - 1;
        if (onChange) onChange(*valuePtr);
      }
    }
    
    const char* getName() const override {
      return name;
    }
};

// ============================================================================================================

#define MAX_ITEMS 10
class Menu : public MenuItem {
  private:
    const char* title;
    MenuItem* items[MAX_ITEMS];
    int itemCount = 0;
    int selectedIndex = 0;
    Menu* parent = nullptr;

  public:
    Menu(const char* t) : title(t) {}

    ItemType getType() const override {
      return MENU_ITEM;
    }

    const char* getName() const override{
      return title;
    }

    // Draw the 'path', so parent menu name > curent menu name
    String getPath() 
    {
      String path = "";
      if(parent != nullptr)
      {
        path = String(String(parent->getName()) + " > ");
      }
      return String(path + title);
    }

    void increase() override {} // Ignore
    void decrease() override {} // Ignore

    MenuItem* getSelectedItem(){
      return items[selectedIndex];
    }

    void addItem(MenuItem* item) {
      if (itemCount < MAX_ITEMS) 
      {
        // When adding another menu as a submenu
        if(item->getType() == MENU_ITEM)
        {
          Menu* submenu = (Menu*)item;
          LOGG_DEBUG("Submenu added");
          if (submenu != nullptr) {
            submenu->setParent(this);
          }
        }
        items[itemCount++] = item;
      }
    }

    void setParent(Menu* p) {
      parent = p;
    }

    Menu* getParent() {
      return parent;
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
      if(selectedIndex < itemCount-1) selectedIndex++;
    }

    void prev() {
      if(selectedIndex > 0) selectedIndex--;
    }


    
};


class MenuManager {
  private:
    Menu* menus[MAX_MENUS];
    int menuCount = 0;
    bool active = false;
    int currentMenuIndex = MENU_SETTINGS;
    Menu* currentMenuPtr;
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
      return currentMenuPtr;// menus[currentMenuIndex];
    }

    int currentMenuID()
    {
      return currentMenuIndex;
    }

    void switchTo(int id) {
      currentMenuIndex = id;
      currentMenuPtr = menus[currentMenuIndex];
      active = true;
      LOGG_DEBUG("Menu: " + String(currentMenuIndex));      
    }

    void exit()
    {
      active = false;
    }

    void enter()
    {
      MenuItem* item = currentMenu()->getSelectedItem();

      if (item->getType() == MENU_ITEM) 
      {
        LOGG_DEBUG("enter");
        currentMenuPtr = (Menu*)item;
        //item.first();
        first();
      }       
    }

    void back() 
    {
      
      if (currentMenuPtr->getParent() != nullptr) 
      {
        // We're in a submenu
        currentMenuPtr = currentMenuPtr->getParent();
      }
      // Otherwise, exit the menu system
      else
        exit();
  }

    // Delegate navigation to active menu
    void first() { currentMenu()->first(); }
    void next() { currentMenu()->next(); }
    void prev() { currentMenu()->prev(); }


};


extern Menu menuSettings;

extern MenuManager menuMgr;

extern void menu_begin();

#endif