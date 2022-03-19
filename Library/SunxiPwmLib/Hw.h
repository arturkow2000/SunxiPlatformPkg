#pragma once

#define PWM_BASE 0x01c20e00

#define PWM_CTRL (PWM_BASE + 0x000)
#define PWM_CH0 (PWM_BASE + 0x004)

#define PWM_CTRL_CH_PRESCALER_SHIFT(ch) (0 << (ch * 16))
#define PWM_CTRL_CH_PRESCALER_MASK(ch) (0xf << (ch * 16))
#define PWM_CTRL_CH_ENABLE(ch) ((1 << 4) << (ch * 16))
#define PWM_CTRL_CH_POLARITY(ch) ((1 << 5) << (ch * 16))
#define PWM_CTRL_CH_GATING(ch) ((1 << 6) << (ch * 16))
#define PWM_CTRL_CH_MODE(ch) ((1 << 7) << (ch * 16))
#define PWM_CTRL_CH_PULSE(ch) ((1 << 8) << (ch * 16))
#define PWM_CTRL_CH_MASK(ch) (0x1ff << (ch * 16))
