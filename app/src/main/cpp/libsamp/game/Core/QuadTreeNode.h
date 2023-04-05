//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_QUADTREENODE_H
#define LIVERUSSIA_QUADTREENODE_H

#include "Rect.h"
#include "Pool.h"
#include "PtrListSingleLink.h"

class CQuadTreeNode;

/*
node level 2
 +----------------------+--
 | node level 1         +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 0 |index 1 | +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 2 |index 3 | +
 |  +--------+--------+ +
 +----------------------+--
 |                      |

 Total rectangles = 4^startLevel
*/

typedef void(*CQuadTreeNodeRectCallBack) (const CRect& rect, void* item);
typedef void(*CQuadTreeNodeVec2DCallBack) (const CVector2D& rect, void* item);
typedef CPool<CQuadTreeNode> CQuadTreeNodePool;

class CQuadTreeNode {
public:
    CRect              m_Rect;
    CPtrListSingleLink m_ItemList;
    CQuadTreeNode*     m_apChildren[4];
    uint32_t           m_nLevel; // 0 - last level

    static CQuadTreeNodePool* ms_pQuadTreeNodePool;

public:
    CQuadTreeNode(const CRect& size, int32_t startLevel);
    ~CQuadTreeNode();

    static void* operator new(unsigned size);
    static void operator delete(void* data);

public:
    static void InjectHooks();

    static void InitPool();

    void AddItem(void* item, const CRect& rect);
    void DeleteItem(void* item);
    void DeleteItem(void* item, const CRect& rect);
    int32_t FindSector(const CRect& rect);
    int32_t FindSector(const CVector2D& posn);
    void ForAllMatching(const CRect& rect, CQuadTreeNodeRectCallBack callback);
    void ForAllMatching(const CVector2D& posn, CQuadTreeNodeVec2DCallBack callback);
    void GetAll(CPtrListSingleLink& list);
    void GetAllMatching(const CRect& rect, CPtrListSingleLink& list);
    void GetAllMatching(const CVector2D& posn, CPtrListSingleLink& list);
    bool InSector(const CRect& rect, int32_t sector) const;

// Helpers
public:
    CRect GetSectorRect(int32_t sector) const;
    bool LiesInside(const CRect& rect) const {
        return    m_Rect.left <= rect.right
                  && m_Rect.right >= rect.left
                  && m_Rect.bottom <= rect.top
                  && m_Rect.top >= rect.bottom;
    };
};


#endif //LIVERUSSIA_QUADTREENODE_H
