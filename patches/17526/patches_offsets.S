# these are manual
#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002D808
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010A758
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613CC
#####################################

.set XexpConvertError, 0x8007b130
.set XexpLoadXexHeadersPatch, 0x8007bcc8
.set BadNetworkNamePatch, 0x8007bdd8
.set ResourceNotFound1Patch, 0x8007be40
.set ResourceNotFound2Patch, 0x8007be90
.set XexpVerifyMinimumVersion, 0x8007a718
.set SfcxFixBBSizePatch, 0x80094638
.set SataCdRomAuthenticationExInitializePatch, 0x80098f90
.set E66xgd3, 0x80098974
.set XeKeysVerifyRSASignature, 0x80109300
.set XeKeysSecurityConvertError, 0x80109df8
.set XeKeysDvdAuthExConvertError, 0x8010a0d8
.set _XeKeysRevokeIsValid, 0x8010a5a0
.set XeKeysRevokeIsRevoked, 0x8010a7a8
.set XeKeysRevokeIsDeviceRevoked, 0x8010a8e8
.set XeKeysRevokeConvertError, 0x8010aa68
.set XeKeysConsoleSignatureVerification, 0x8010b590
.set XeCryptBnQwBeSigVerify, 0x80111000
.set SataDiskAuthenticateDevice, 0x8015d0c0
.set SataDiskAuthenticateDeviceInst1, 0x7d8802a6
.set SataDiskAuthenticateDeviceInst2, 0x4bfaf179
.set SataDiskAuthenticateDeviceInv1, 0x8015d1e4
.set SataDiskAuthenticateDeviceInv2, 0x8015d1f0
.set XexLoadImage, 0x8007cf10
.set XexLoadExecutablePatch, 0x8007cf48
.set xferMassConfigureTransferCable1, 0x800e0dc4
.set xferMassConfigureTransferCable1dest, 0x800e0e18
.set xferMassConfigureTransferCable2, 0x800e0e3c
.set AddonNoNandMuPatch, 0x8008d798
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x8015b388
.set AddonNoHdmiWaitDestPatch, 0x8015b3d4
.set SataChannelDetectDevice, 0x8015d418
# STATUS: named SataDiskInitialize at 0x8015d200
.set AddonNoTrinMuPatch, 0x800e1c18
.set AddonNoWiredLanPatch, 0x800d153c
# STATUS: named NicDriverEntry at 0x800d1518
.set NomnilDetectDevicePatch, 0x8014adb0
.set OmniDetectDevicePatch, 0x80133860
.set AddonNoCorMu, 0x8008eaf8
.set HvxKeysGetKeyProperties_2fPatch, 0x80107a80
.set XeKeysGetKeyProperties, 0x801084e0

# -----------dash launch kernel autos----------- #
# .set dl_DebugStepP, 0x800727dc
# .set dl_KiDebugRoutine, 0x801a58c4
# .set dl_DebugTrap, 0x8015ae28
