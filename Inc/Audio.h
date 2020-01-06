#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>
#include <stdbool.h>

typedef void AudioCallbackFunction(void *context,int buffer);

#define Audio8000HzSettings 256,5,12,1
#define Audio16000HzSettings 213,2,13,0
#define Audio32000HzSettings 213,2,6,1
#define Audio48000HzSettings 258,3,3,1
#define Audio96000HzSettings 344,2,3,1
#define Audio22050HzSettings 429,4,9,1
#define Audio44100HzSettings 271,2,6,0
#define AudioVGAHSyncSettings 419,2,13,0 // 31475.3606. Actual VGA timer is 31472.4616.

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

/** CS43l22 Registers  ***/
#define   CS43L22_REG_ID                  0x01
#define   CS43L22_REG_POWER_CTL1          0x02
#define   CS43L22_REG_POWER_CTL2          0x04
#define   CS43L22_REG_CLOCKING_CTL        0x05
#define   CS43L22_REG_INTERFACE_CTL1      0x06
#define   CS43L22_REG_INTERFACE_CTL2      0x07
#define   CS43L22_REG_PASSTHR_A_SELECT    0x08
#define   CS43L22_REG_PASSTHR_B_SELECT    0x09
#define   CS43L22_REG_ANALOG_ZC_SR_SETT   0x0A
#define   CS43L22_REG_PASSTHR_GANG_CTL    0x0C
#define   CS43L22_REG_PLAYBACK_CTL1       0x0D
#define   CS43L22_REG_MISC_CTL            0x0E
#define   CS43L22_REG_PLAYBACK_CTL2       0x0F
#define   CS43L22_REG_PASSTHR_A_VOL       0x14
#define   CS43L22_REG_PASSTHR_B_VOL       0x15
#define   CS43L22_REG_PCMA_VOL            0x1A
#define   CS43L22_REG_PCMB_VOL            0x1B
#define   CS43L22_REG_BEEP_FREQ_ON_TIME   0x1C
#define   CS43L22_REG_BEEP_VOL_OFF_TIME   0x1D
#define   CS43L22_REG_BEEP_TONE_CFG       0x1E
#define   CS43L22_REG_TONE_CTL            0x1F
#define   CS43L22_REG_MASTER_A_VOL        0x20
#define   CS43L22_REG_MASTER_B_VOL        0x21
#define   CS43L22_REG_HEADPHONE_A_VOL     0x22
#define   CS43L22_REG_HEADPHONE_B_VOL     0x23
#define   CS43L22_REG_SPEAKER_A_VOL       0x24
#define   CS43L22_REG_SPEAKER_B_VOL       0x25
#define   CS43L22_REG_CH_MIXER_SWAP       0x26
#define   CS43L22_REG_LIMIT_CTL1          0x27
#define   CS43L22_REG_LIMIT_CTL2          0x28
#define   CS43L22_REG_LIMIT_ATTACK_RATE   0x29
#define   CS43L22_REG_OVF_CLK_STATUS      0x2E
#define   CS43L22_REG_BATT_COMPENSATION   0x2F
#define   CS43L22_REG_VP_BATTERY_LEVEL    0x30
#define   CS43L22_REG_SPEAKER_STATUS      0x31
#define   CS43L22_REG_TEMPMONITOR_CTL     0x32
#define   CS43L22_REG_THERMAL_FOLDBACK    0x33
#define   CS43L22_REG_CHARGE_PUMP_FREQ    0x34

/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

int volume;


// Initialize and power up audio hardware. Use the above defines for the parameters.
// Can probably only be called once.
void InitializeAudio(int plln,int pllr,int i2sdiv,int i2sodd);

// Power up and down the audio hardware.
void AudioOn();
void AudioOff();

// Set audio volume in steps of 0.5 dB. 0xff is +12 dB.
void SetAudioVolume(int volume);

// Output one audio sample directly to the hardware without using DMA.
void OutputAudioSample(int16_t sample);
void OutputAudioSampleWithoutBlocking(int16_t sample);

// Start and stop audio playback using DMA.
// Callback is optional, and called whenever a new buffer is needed.
void PlayAudioWithCallback(AudioCallbackFunction *callback,void *context);
void StopAudio();

// Provide a new buffer to the audio DMA. Output is double buffered, so
// at least two buffers must be maintained by the program. It is not allowed
// to overwrite the previously provided buffer until after the next callback
// invocation.
// Buffers must reside in DMA1-accessible memory, that is, the 128k RAM bank,
// or flash.
void ProvideAudioBuffer(void *samples,int numsamples);
bool ProvideAudioBufferWithoutBlocking(void *samples,int numsamples);
void StopAudioDMA();
void Audio_SetMute(uint32_t Cmd);
void Audio_Pause();
void Audio_Resume();

#endif
