//
// Created by plaka on 18.11.2022.
//

#ifndef LIVERUSSIA_CINVENTORY_H
#define LIVERUSSIA_CINVENTORY_H

static class CInventory {

public:
    static void toggleShow(bool toggle, float satiety);

    static bool bIsToggle;
    static jobject thiz;

    static void updateItem(int matrixindex, int pos, const char sprite[], const char caption[], bool active);

    static void updateItem(int matrixindex, int pos, bool active);

    static void updateCarryng(int matrixindex, int pos, int active);
};

#endif //LIVERUSSIA_CINVENTORY_H
