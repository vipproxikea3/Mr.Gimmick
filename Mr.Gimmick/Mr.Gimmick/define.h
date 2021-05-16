#pragma once

//INTRO SCENE:

#define INTRO_SCENE_ID 999

#define INTRO_ANI_ID							70001
#define INTRO_CAKE_ANI_ID						70002
#define INTRO_GIRL_BLOW_ANI_ID					70003
#define INTRO_LIGHTNING_DISSAPEAR_ANI_ID		70004//0.6s
#define INTRO_GIMMICK_ANI_IDLE					70005
#define INTRO_GIMMICK_ANI_FALL					70006
#define INTRO_GIMMICK_ANI_BOUNCE				70007
#define INTRO_GIMMICK_ANI_GETUP					70008
#define INTRO_GIMMICK_ANI_IDLE_LOOKUP			70009
#define INTRO_GIMMICK_ANI_JUMP					70010
#define INTRO_GATE_ANI_ID					70011


#define INTRO_GIRL_BLOW_SPRITE_ID	70003
#define INTRO_WIDTH		128
#define INTRO_HEIGHT	80

#define FRAME_1_TIME						3000
#define FRAME_CAKE_TIME						2000
#define FRAME_GIRL_BLOW_TIME				1000
#define FRAME_12_DISSAPEAR_START_TIME		35000
#define FRAME_12_DISSAPEAR_TIME				600
#define FRAME_12_GIMMICK_START_TIME			35000
#define FRAME_12_GIMMICK_TIME				8350 //add 0.35s delay
#define FRAME_12_GIMMICK_FALL_START_TIME	37500
#define FRAME_14_GIMMICK_START_TIME			47350 //add 0.35s delay
#define FRAME_14_GIMMICK_TIME				2500 
#define FRAME_14_GIMMICK_LOOKUP_TIME		2000
#define FRAME_14_GATE_OPEN_START_TIME		47000
#define FRAME_14_GATE_TIME					3000 //1.35s

#define INTRO_GIMMICK_STATE_IDLE			1
#define INTRO_GIMMICK_STATE_FALL			2
#define INTRO_GIMMICK_STATE_BOUNCE			3
#define INTRO_GIMMICK_STATE_GETUP			4
#define INTRO_GIMMICK_STATE_JUMP			5
#define INTRO_GIMMICK_STATE_IDLE_LOOKUP			6

#define INTRO_GIMMICK_START_X				48
#define INTRO_GIMMICK_START_Y				0
#define INTRO_GIMMICK_STOP_FALL_Y			60 //55
#define INTRO_BOUNCE_RANGE					12
#define INTRO_SMALL_BOUNCE_RANGE			1
#define INTRO_JUMP_RANGE_Y					35

#define INTRO_GIMMICK_HEIGHT				10
