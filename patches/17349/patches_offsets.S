# these are manual
#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002D7E0
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010C088
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613D0
#####################################

.set XexpConvertError, 0x8007b198
.set XexpLoadXexHeadersPatch, 0x8007bd30
.set BadNetworkNamePatch, 0x8007be40
.set ResourceNotFound1Patch, 0x8007bea8
.set ResourceNotFound2Patch, 0x8007bef8
.set XexpVerifyMinimumVersion, 0x8007a778
.set SfcxFixBBSizePatch, 0x800945a0
.set SataCdRomAuthenticationExInitializePatch, 0x80098f30
.set E66xgd3, 0x80098914
.set XeKeysVerifyRSASignature, 0x8010ac30
.set XeKeysSecurityConvertError, 0x8010b728
.set XeKeysDvdAuthExConvertError, 0x8010ba08
.set _XeKeysRevokeIsValid, 0x8010bed0
.set XeKeysRevokeIsRevoked, 0x8010c0d8
.set XeKeysRevokeIsDeviceRevoked, 0x8010c218
.set XeKeysRevokeConvertError, 0x8010c398
.set XeKeysConsoleSignatureVerification, 0x8010cec0
.set XeCryptBnQwBeSigVerify, 0x80111860
.set SataDiskAuthenticateDevice, 0x8015a578
.set SataDiskAuthenticateDeviceInst1, 0x7d8802a6
.set SataDiskAuthenticateDeviceInst2, 0x4bfb35f1
.set SataDiskAuthenticateDeviceInv1, 0x8015a69c
.set SataDiskAuthenticateDeviceInv2, 0x8015a6a8
.set XexLoadImage, 0x8007cf78
.set XexLoadExecutablePatch, 0x8007cfb0
.set xferMassConfigureTransferCable1, 0x800e2744
.set xferMassConfigureTransferCable1dest, 0x800e2798
.set xferMassConfigureTransferCable2, 0x800e27bc
.set AddonNoNandMuPatch, 0x8008d7e8
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x80158840
.set AddonNoHdmiWaitDestPatch, 0x8015888c
.set SataChannelDetectDevice, 0x8015a8d0
# STATUS: named SataDiskInitialize at 0x8015a6b8
.set AddonNoTrinMuPatch, 0x800e3598
.set AddonNoWiredLanPatch, 0x800d2eac
# STATUS: named NicDriverEntry at 0x800d2e88
.set NomnilDetectDevicePatch, 0x80141680
.set OmniDetectDevicePatch, 0x80125f40
.set AddonNoCorMu, 0x8008eb48
.set HvxKeysGetKeyProperties_2fPatch, 0x801093b0
.set XeKeysGetKeyProperties, 0x80109e10

# -----------dash launch kernel autos----------- #
# .set dl_DebugStepP, 0x8007281c
# .set dl_KiDebugRoutine, 0x801993a4
# .set dl_DebugTrap, 0x80156f38


