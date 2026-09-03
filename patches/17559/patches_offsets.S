# these are manual
#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002E240
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010B0E8
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613CC
#####################################

.set XexpConvertError, 0x8007b920
.set XexpLoadXexHeadersPatch, 0x8007c4b8
.set BadNetworkNamePatch, 0x8007C5E8
.set ResourceNotFound1Patch, 0x8007C634
.set ResourceNotFound2Patch, 0x8007C684
.set XexpVerifyMinimumVersion, 0x8007af08
.set SfcxFixBBSizePatch, 0x80094f78
.set SataCdRomAuthenticationExInitializePatch, 0x800998d0
.set E66xgd3, 0x800992b4
.set XeKeysVerifyRSASignature, 0x80109c90
.set XeKeysSecurityConvertError, 0x8010a788
.set XeKeysDvdAuthExConvertError, 0x8010aa68
.set _XeKeysRevokeIsValid, 0x8010af30
.set XeKeysRevokeIsRevoked, 0x8010b138
.set XeKeysRevokeIsDeviceRevoked, 0x8010b278
.set XeKeysRevokeConvertError, 0x8010b3f8
.set XeKeysConsoleSignatureVerification, 0x8010bf20
.set XeCryptBnQwBeSigVerify, 0x80111990
.set SataDiskAuthenticateDevice, 0x8015d9d8
.set SataDiskAuthenticateDeviceInst1, 0x7d8802a6
.set SataDiskAuthenticateDeviceInst2, 0x4bfaf1f1
.set SataDiskAuthenticateDeviceInv1, 0x8015dafc
.set SataDiskAuthenticateDeviceInv2, 0x8015db08
.set XexLoadImage, 0x8007d7c0
.set XexLoadExecutablePatch, 0x8007d7f8
.set xferMassConfigureTransferCable1, 0x800e1754
.set xferMassConfigureTransferCable1dest, 0x800e17a8
.set xferMassConfigureTransferCable2, 0x800e17cc
.set AddonNoNandMuPatch, 0x8008e098
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x8015bc80
.set AddonNoHdmiWaitDestPatch, 0x8015bccc
.set SataChannelDetectDevice, 0x8015dd30
# STATUS: named SataDiskInitialize at 0x8015db18
.set AddonNoTrinMuPatch, 0x800e25a8
.set AddonNoWiredLanPatch, 0x800d1ee4
# STATUS: named NicDriverEntry at 0x800d1ec0
.set NomnilDetectDevicePatch, 0x8014b778
.set OmniDetectDevicePatch, 0x801341f0
.set AddonNoCorMu, 0x8008f3f8
.set HvxKeysGetKeyProperties_31Patch, 0x80108410
.set XeKeysGetKeyProperties, 0x80108e70

# -----------dash launch kernel autos----------- #
# .set dl_DebugStepP, 0x800727dc
# .set dl_KiDebugRoutine, 0x801a58c4
# .set dl_DebugTrap, 0x8015b720

