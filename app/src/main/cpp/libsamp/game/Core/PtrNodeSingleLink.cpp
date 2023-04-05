//
// Created by plaka on 05.04.2023.
//

#include "PtrNodeSingleLink.h"

void CPtrNodeSingleLink::AddToList(CPtrListSingleLink* list) {
   // m_next = list->GetNode();
   // list->m_node = reinterpret_cast<CPtrNode*>(this);
}

void* CPtrNodeSingleLink::operator new(unsigned size) {
  //  return GetPtrNodeSingleLinkPool()->New();
}

void CPtrNodeSingleLink::operator delete(void* data) {
  //  GetPtrNodeSingleLinkPool()->Delete(static_cast<CPtrNodeSingleLink*>(data));
}