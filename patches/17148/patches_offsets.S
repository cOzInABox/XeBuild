# these are manual
#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002D7E0
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010BF68
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613D0
#####################################
# sata addon no ss hdd allowed
#####################################
.set SataDiskAuthenticateDeviceInv1, 0x8015A55C
.set SataDiskAuthenticateDeviceInv2, 0x8015A568
.set SataDiskAuthenticateDeviceInst1, 0x7D8802A6
.set SataDiskAuthenticateDeviceInst2, 0x4BFB3611
#####################################
# when unsigned drive is in transfer cable
#####################################
.set xferMassConfigureTransferCable1, 0x800E26BC		# auth
.set xferMassConfigureTransferCable1dest, 0x800E2710
.set xferMassConfigureTransferCable2, 0x800E2734		# size
#####################################

.set XexpConvertError, 0x8007b150
.set XexpLoadXexHeadersPatch, 0x8007bce8
.set BadNetworkNamePatch, 0x8007bdf8
.set ResourceNotFound1Patch, 0x8007be60
.set ResourceNotFound2Patch, 0x8007beb0
.set XexpVerifyMinimumVersion, 0x8007a738
.set SfcxFixBBSizePatch, 0x80094510
.set SataCdRomAuthenticationExInitializePatch, 0x80098e18
.set E66xgd3, 0x800987fc
.set XeKeysVerifyRSASignature, 0x8010ab10
.set XeKeysSecurityConvertError, 0x8010b608
.set XeKeysDvdAuthExConvertError, 0x8010b8e8
.set _XeKeysRevokeIsValid, 0x8010bdb0
.set XeKeysRevokeIsRevoked, 0x8010bfb8
.set XeKeysRevokeIsDeviceRevoked, 0x8010c0f8
.set XeKeysRevokeConvertError, 0x8010c278
.set XeKeysConsoleSignatureVerification, 0x8010cda0
.set XeCryptBnQwBeSigVerify, 0x80111740
.set SataDiskAuthenticateDevice, 0x8015a438
.set XexLoadImage, 0x8007cf30
.set XexLoadExecutablePatch, 0x8007cf68
.set AddonNoNandMuPatch, 0x8008d6a8
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x80158700
.set AddonNoHdmiWaitDestPatch, 0x8015874c
.set SataChannelDetectDevice, 0x8015a790
# STATUS: named SataDiskInitialize at 0x8015a578
.set AddonNoTrinMuPatch, 0x800e3510
.set AddonNoWiredLanPatch, 0x800d2dd4
# STATUS: named NicDriverEntry at 0x800d2db0
.set AddonNoCorMu, 0x8008e9d0
.set HvxKeysGetKeyProperties_2fPatch, 0x801092a0
.set XeKeysGetKeyProperties, 0x80109cf0
