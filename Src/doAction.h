#ifndef MY_DOACTION_H
#define MY_DOACTION_H

#ifdef __cplusplus
extern "C" {
#endif

    extern int doDetab( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doDump( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doExt2Txt( tConvmArguments *pContextArg, tContextApp *pContextApp);

    extern int doRleComDecom( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doToBmp( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doToPic( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doNumberOfColor_NotUsePerLine( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doBMP_NumberOfColor_NotUsePerLine( tConvmArguments* pContextArg, tContextApp* pContextApp, enum eCommandNumber eCommand);

    extern int doAddPaletteToBmp4( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doInsertPaletteToBmp8(tConvmArguments* pContextArg, tContextApp* pContextApp, enum eCommandNumber eCommand);

    extern int doSwapTwoColor( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doComparePalette( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand);

    extern int doCopyPalette(tConvmArguments* pContextArg, tContextApp* pContextApp, enum eCommandNumber eCommand);

    extern int doExtSprite(tConvmArguments* pContextArg, tContextApp* pContextApp, enum eCommandNumber eCommand);


#ifdef __cplusplus
}
#endif

#endif
