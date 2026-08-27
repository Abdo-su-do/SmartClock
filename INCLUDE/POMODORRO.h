/*
 * POMODORRO.h
 *
 *  Created on: ??˛/??˛/????
 *      Author: ALRYADA
 */

#ifndef INCLUDE_MCAL_POMODORRO_H_
#define INCLUDE_MCAL_POMODORRO_H_



#include "lib/STD_TYPES.h"
#include "HAL/LCD/LCD_INTERFACE.h" // ··Ê’Ê· ·‹ LCD_POMODORO_STATUS

/* ≈⁄œ«œ«  √Êﬁ«  «·»Ê„ÊœÊ—Ê «·«› —«÷Ì… */
#define POMO_WORK_MINUTES           0
#define POMO_WORK_SECONDS           3
#define POMO_BREAK_MINUTES          0       /* Short Break */
#define POMO_BREAK_SECONDS          3
#define POMO_LONG_BREAK_MINUTES     15      /* Long Break */
#define POMO_LONG_BREAK_SECONDS     0
#define POMO_MAX_SESSIONS           4       /* ⁄œœ «·”Ì‘‰“ ﬁ»· «·—«Õ… «·ÿÊÌ·… */

#define POMO_MODE_WORK              0
#define POMO_MODE_SHORT_BREAK       1
#define POMO_MODE_LONG_BREAK        2
#define POMO_MODE_SETTING           3

void POMODORRO_voidInit(void);
void POMODORRO_voidUpdate(void);
void POMODORRO_voidStartPause(void);
void POMODORRO_voidReset(void);
void POMODORRO_voidHandleKeypadInput(u8 a_u8Key);
void POMODORRO_voidRender(void);

#endif /* INCLUDE_MCAL_POMODORRO_H_ */
