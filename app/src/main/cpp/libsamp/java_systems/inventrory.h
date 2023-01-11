//
// Created by plaka on 18.11.2022.
//

#ifndef LIVERUSSIA_INVENTRORY_H
#define LIVERUSSIA_INVENTRORY_H

class CINVENTORY {

public:
    void ToggleShow(bool toggle, float satiety);

    bool bIsToggle = false;

    void InventoryUpdateItem(int matrixindex, int pos, const char sprite[], const char caption[], bool active);

    void UpdateItem(int matrixindex, int pos, bool active);

    void UpdateCarryng(int matrixindex, int pos, int active);
};

#endif //LIVERUSSIA_INVENTRORY_H
