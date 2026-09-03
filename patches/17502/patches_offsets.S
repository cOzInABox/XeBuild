# these are manual
#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002D808
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010C138
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613CC
#####################################

.set XexpConvertError, 0x8007b160
.set XexpLoadXexHeadersPatch, 0x8007bcf8
.set BadNetworkNamePatch, 0x8007be08
.set ResourceNotFound1Patch, 0x8007be70
.set ResourceNotFound2Patch, 0x8007bec0
.set XexpVerifyMinimumVersion, 0x8007a748
.set SfcxFixBBSizePatch, 0x800945f0
.set SataCdRomAuthenticationExInitializePatch, 0x80098f18
.set E66xgd3, 0x800988fc
.set XeKeysVerifyRSASignature, 0x8010ace0
.set XeKeysSecurityConvertError, 0x8010b7d8
.set XeKeysDvdAuthExConvertError, 0x8010bab8
.set _XeKeysRevokeIsValid, 0x8010bf80
.set XeKeysRevokeIsRevoked, 0x8010c188
.set XeKeysRevokeIsDeviceRevoked, 0x8010c2c8
.set XeKeysRevokeConvertError, 0x8010c448
.set XeKeysConsoleSignatureVerification, 0x8010cf70
.set XeCryptBnQwBeSigVerify, 0x801129e0
.set SataDiskAuthenticateDevice, 0x8015ea70
.set SataDiskAuthenticateDeviceInst1, 0x7d8802a6
.set SataDiskAuthenticateDeviceInst2, 0x4bfaf1a9
.set SataDiskAuthenticateDeviceInv1, 0x8015eb94
.set SataDiskAuthenticateDeviceInv2, 0x8015eba0
.set XexLoadImage, 0x8007cf40
.set XexLoadExecutablePatch, 0x8007cf78
.set xferMassConfigureTransferCable1, 0x800e27a4
.set xferMassConfigureTransferCable1dest, 0x800e27f8
.set xferMassConfigureTransferCable2, 0x800e281c
.set AddonNoNandMuPatch, 0x8008d838
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x8015cd38
.set AddonNoHdmiWaitDestPatch, 0x8015cd84
.set SataChannelDetectDevice, 0x8015edc8
# STATUS: named SataDiskInitialize at 0x8015ebb0
.set AddonNoTrinMuPatch, 0x800e35f8
.set AddonNoWiredLanPatch, 0x800d2f34
# STATUS: named NicDriverEntry at 0x800d2f10
.set NomnilDetectDevicePatch, 0x8014c760
.set OmniDetectDevicePatch, 0x80135238
.set AddonNoCorMu, 0x8008eb98
.set HvxKeysGetKeyProperties_2fPatch, 0x80109460
.set XeKeysGetKeyProperties, 0x80109ec0

# -----------dash launch kernel autos----------- #
# .set dl_DebugStepP, 0x800727ec
# .set dl_KiDebugRoutine, 0x801a58c4
# .set dl_DebugTrap, 0x8015c7d8



