//
// Created by plaka on 01.02.2023.
//

#ifndef LIVERUSSIA_CDIALOG_H
#define LIVERUSSIA_CDIALOG_H


#include <jni.h>
#include "raknet/NetworkTypes.h"

static class CDialog {
public:
    static jobject thiz;
    static bool bIsShow;

    static void show(int id, int style, char *caption, char *info, char *button1, char *button2);

    static void rpcShowPlayerDialog(RPCParameters *rpcParams);
};


#endif //LIVERUSSIA_CDIALOG_H
