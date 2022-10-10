
; NES System Defines
; Contains various register definitions commonly used in NES development


;
; PPU Registers            
;

.define PPU_CTRL $2000
.define PPU_MASK $2001
.define PPU_STATUS $2002
.define OAM_ADDR $2003
.define OAM_DATA $2004
.define PPU_SCROLL $2005
.define PPU_ADDR $2006
.define PPU_DATA $2007
.define OAM_DMA $4014


;
; APU Registers            
;

.define APU_PULSE_1_DUTY $4000
.define APU_PULSE_1_SWEEP $4001
.define APU_PULSE_1_TIMER_LOW $4002
.define APU_PULSE_1_LEN_TIMER $4003
.define APU_PULSE_2_DUTY $4004
.define APU_PULSE_2_SWEEP $4005
.define APU_PULSE_2_TIMER_LOW $4006
.define APU_PULSE_2_LEN_TIMER $4007
.define APU_TRIANGLE_LEN $4008
.define APU_TRIANGLE_TIMER_LOW $400a
.define APU_TRIANGLE_LEN_TIMER $400b
.define APU_NOISE_LEN $400f
.define APU_NOISE_LOOP $400e
.define APU_DMC_IRQ $4010
.define APU_DMC_DIRECT_LOAD $4011
.define APU_DMC_SAMPLE_ADDR $4012
.define APU_DMC_SAMPLE_LEN $4013
.define APU_STATUS $4015
.define APU_FRAME_COUNTER $4017


;
; Controller Registers            
;

.define CTRL_PORT_1 $4016
.define CTRL_PORT_2 $4017
