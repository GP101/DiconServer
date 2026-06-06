#include "KUser.h"
#include <string.h>

void KUser::SendText(const char* text)
{
    int len = strlen(text);
    m_pSocket->SendData(text, len);
}
