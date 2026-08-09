#ifndef MY_MIDI_SYNTH_H
#define MY_MIDI_SYNTH_H

#pragma once


/****************************************************************
 * STRUCTURE
 ***************************************************************/

 /**
  * @struct aSequenceFileHeader
  * @brief  Structure of header of sequence file format .seq file with file type 0xd5 and aux type 0x0001
  */
typedef struct aSequenceFileHeader
{
    unsigned long   ulType;                 /**< type of sequence file - "MSEQ" */
    unsigned short  uiVersion;              /**< version of sequence file - 0x0100 */
    unsigned short  uiOffestToSeqData;      /**< offset to sequence data */
    unsigned char   sOwnerName[16];         /**< owner name of sequence file */
    unsigned char   sInstrumentName[16];    /**< instrument name of sequence file */
    unsigned char   sTrack_0_Name[16];      /**< track 0 name of sequence file */
    unsigned char   sTrack_1_Name[16];      /**< track 1 name of sequence file */
    unsigned char   sTrack_2_Name[16];      /**< track 2 name of sequence file */
    unsigned char   sTrack_3_Name[16];      /**< track 3 name of sequence file */
    unsigned char   sTrack_4_Name[16];      /**< track 4 name of sequence file */
    unsigned char   sTrack_5_Name[16];      /**< track 5 name of sequence file */
    unsigned char   sTrack_6_Name[16];      /**< track 6 name of sequence file */
    unsigned char   sTrack_7_Name[16];      /**< track 7 name of sequence file */
    unsigned char   sTrack_8_Name[16];      /**< track 8 name of sequence file */
    unsigned char   sTrack_9_Name[16];      /**< track 9 name of sequence file */
    unsigned char   sTrack_A_Name[16];      /**< track 10 name of sequence file */
    unsigned char   sTrack_B_Name[16];      /**< track 11 name of sequence file */
    unsigned char   sTrack_C_Name[16];      /**< track 12 name of sequence file */
    unsigned char   sTrack_D_Name[16];      /**< track 13 name of sequence file */
    unsigned char   sTrack_E_Name[16];      /**< track 14 name of sequence file */
    unsigned char   sTrack_F_Name[16];      /**< track 15 name of sequence file */
} aSequenceFileHeader, *pSequenceFileHeader;

/**
 * @struct aSequenceInitialization
 * @brief  Structure of after header of sequence Initialization
 */
typedef struct aSequenceInitialization
{
    unsigned short  uChannelVolumes[16];        /**< channel volumes of sequence file */
    unsigned short  uTrackChannel[16];          /**< track channels of sequence file */
    unsigned short  uTrackPlay[16];             /**< track play status of sequence file */
    unsigned short  uRecordTrack;               /**< record track of sequence file */
    unsigned short  uTempo;                     /**< tempo of sequence file */
    unsigned short  uBeatsPerMeasure;           /**< beats per measure of sequence file */
    unsigned short  uBeatValue;                 /**< beat value of sequence file */
    unsigned short  uTrickPerBeat;              /**< trick per beat of sequence file */
    unsigned short  uTrackOutputMap[16];        /**< track output map of sequence file */
    unsigned short  uReserved[16];              /**< reserved of sequence file */
} aSequenceInitialization, *pSequenceInitialization;


/**
 * @struct aInstrumentFileHeader
 * @brief  Structure of header of instrument file format .ins file with file type 0xd6 and aux type 0x0001
 */
typedef struct aInstrumentFileHeader
{
    unsigned long   ulType;                     /**< type of instrument file - "INST" */
    unsigned short  uiVersion;                  /**< version of instrument file - 0x0100 */
    unsigned short  uiHeaderSize;               /**< size of instrument file header */
    unsigned char   sOwnerName[16];             /**< owner name of instrument file -> Not used in synthLab */
    unsigned char   sWaveFileName[16];          /**< name of wave file associated with instrument file */
    unsigned char   uMasterSemiToneTuning;      /**< Master Semi-Tone Tuning -> Not used in synthLab */
    unsigned char   uMasterFineTuning;          /**< Master Fine Tuning -> Not used in synthLab */
    unsigned char   uReserved;                  /**< Reserved byte */
    unsigned char   uMasterVolume;              /**< Master Volume 0 - 15 -> Not used in synthLab */
    unsigned char   uNumberOfInstruments;       /**< number of instruments 1 - 16 -> Not used in synthLab */
    unsigned short  uiReserved;                 /**< Reserved word */
    unsigned char   uWaveRefBlock[512];         /**< Wave Reference Block */
    unsigned char   uFree[208];                 /**< Free space in instrument file header */
    unsigned char   sInstrument_0_Name[16];     /**< Instrument 0 name of sequence file */
    unsigned char   sInstrument_1_Name[16];     /**< Instrument 1 name of sequence file */
    unsigned char   sInstrument_2_Name[16];     /**< Instrument 2 name of sequence file */
    unsigned char   sInstrument_3_Name[16];     /**< Instrument 3 name of sequence file */
    unsigned char   sInstrument_4_Name[16];     /**< Instrument 4 name of sequence file */
    unsigned char   sInstrument_5_Name[16];     /**< Instrument 5 name of sequence file */
    unsigned char   sInstrument_6_Name[16];     /**< Instrument 6 name of sequence file */
    unsigned char   sInstrument_7_Name[16];     /**< Instrument 7 name of sequence file */
    unsigned char   sInstrument_8_Name[16];     /**< Instrument 8 name of sequence file */
    unsigned char   sInstrument_9_Name[16];     /**< Instrument 9 name of sequence file */
    unsigned char   sInstrument_A_Name[16];     /**< Instrument 10 name of sequence file */
    unsigned char   sInstrument_B_Name[16];     /**< Instrument 11 name of sequence file */
    unsigned char   sInstrument_C_Name[16];     /**< Instrument 12 name of sequence file */
    unsigned char   sInstrument_D_Name[16];     /**< Instrument 13 name of sequence file */
    unsigned char   sInstrument_E_Name[16];     /**< Instrument 14 name of sequence file */
    unsigned char   sInstrument_F_Name[16];     /**< Instrument 15 name of sequence file */
} aInstrumentFileHeader, *pInstrumentFileHeader;


/**
 * @struct aWaveFileHeader
 * @brief  Structure of header of wave file format .wav file with file type 0xd8 and aux type 0x0004
 */
typedef struct aWaveFileHeader
{
    unsigned long   ulType;                     /**< type of wave file - "WAVE" */
    unsigned short  uiVersion;                  /**< version of wave file - 0x0100 */
    unsigned short  uiOffestToWaveData;         /**< offset to wave data */
    unsigned char   sOwnerName[16];             /**< owner name of wave file -> Not used in synthLab */
    unsigned short  uiNumberOfValidWaveDefs;    /**< number of valid wave definitions in the wave file */
    unsigned char   sFree[230];                 /**< free space in wave file header -> Not used in synthLab */
} aWaveFileHeader, *pWaveFileHeader;

/**
 * @struct aWaveDefinedRecord
 * @brief  Structure of wave defined record
 */
typedef struct aWaveDefinedRecord
{
    unsigned char   sWaveName[16];             /**< wave name of wave defined record */
    unsigned short  uiDocAddress;              /**< DOC address */
    unsigned char   uSetToZero;                /**< Set to zero */
    unsigned char   uSize;                     /**< Sise */
    unsigned char   uVolume;                   /**< Volume -> Not used in synthLab */
    unsigned char   uOctaveTuning;             /**< Octave tuning -> Not used in synthLab */
    unsigned char   uSemiToneTuning;           /**< Semi-tone tuning -> Not used in synthLab */
    unsigned char   uFineTuning;               /**< fine tuning -> Not used in synthLab */
    unsigned char   uFree[8];                  /**< free space in wave file header -> Not used in synthLab */
} aWaveDefinedRecord, *pWaveDefinedRecord;

#ifdef __cplusplus
extern "C" {
#endif

extern int doDumpMidiSynth( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

#ifdef __cplusplus
}
#endif

#endif
