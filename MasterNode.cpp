#include "MasterNode.hpp"

String MasterNode::generateUID()
{
    /* Change to allowable characters */
    const char possible[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static char uid[UID_LENGTH + 1];
    for (int p = 0, i = 0; i < UID_LENGTH; i++)
    {
        int r = random(0, strlen(possible));
        uid[p++] = possible[r];
    }
    uid[UID_LENGTH] = '\0';
    return String(uid);
}

int MasterNode::handleConnection(Packet &packet)
{
    char message = packet.data[1];
    switch (message)
    {
    case REQUEST:
        byte address = packet.data[0];
        String uid = packet.data.substring(2, packet.data.length());
         
        break;
    default:
    }
}