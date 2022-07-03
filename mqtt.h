#ifndef __MAIN_TTU_JZQ__
#define __MAIN_TTU_JZQ__

#include <string>

#define MAIN_DEBUG_EN 1



#define TOPIC_TO_IO_BOARD_MSG "TO_IO_BOARD_MSG"
#define TOPIC_FROM_IO_BOARD_MSG "FROM_IO_BOARD_MSG"



extern int connectMQTT( std::string ipIn, std::string portIn);

extern void Mosq_Send(char *topic,char *buf,int len);
extern void Mosq_SendPrivateTopic(char *buf, int len);
#endif

