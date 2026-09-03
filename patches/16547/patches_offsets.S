# these are manual
#  NicAttach + 8 if the instruction is mr r28, r4
.set NicAttachWait, 0x80104B38

#####################################
#;   disable kv policy for dvd drives
#####################################
.set NoFcrtKvPatch, 0x0002D7E0
#####################################
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#####################################
.set XeKeysRevokeIsRevokedp, 0x8010BEF0
#####################################
# XamLoadedHook
#####################################
.set XamLoadedHookPatch, 0x800613D0
#####################################


.set XexpConvertError, 0x8007b150
.set XexpLoadXexHeadersPatch, 0x8007bce8
.set BadNetworkNamePatch, 0x8007bdf8
.set ResourceNotFound1Patch, 0x8007be60
.set ResourceNotFound2Patch, 0x8007beb0
.set XexpVerifyMinimumVersion, 0x8007a738
.set SfcxFixBBSizePatch, 0x80094428
.set SataCdRomAuthenticationExInitializePatch, 0x80098d80
.set E66xgd3, 0x80098764
.set XeKeysVerifyRSASignature, 0x8010aa98
.set XeKeysSecurityConvertError, 0x8010b590
.set XeKeysDvdAuthExConvertError, 0x8010b870
.set _XeKeysRevokeIsValid, 0x8010bd38
.set XeKeysRevokeIsRevoked, 0x8010bf40
.set XeKeysRevokeIsDeviceRevoked, 0x8010c080
.set XeKeysRevokeConvertError, 0x8010c200
.set XeKeysConsoleSignatureVerificationPatch, 0x8010cd78
.set XeKeysConsoleSignatureVerificationDestPatch, 0x8010cda0
.set XeCryptBnQwBeSigVerify, 0x801116d0
.set SataDiskAuthenticateDevice, 0x8015a2c8
.set XexLoadImage, 0x8007cf30
.set XexLoadExecutablePatch, 0x8007cf68
.set AddonNoNandMuPatch, 0x8008d6a8
.set AddonNoFcrtPhase1Patch, 0x800611a0
.set AddonNoHdmiWaitPatch, 0x80158590
.set AddonNoHdmiWaitDestPatch, 0x801585dc
.set SataChannelDetectDevice, 0x8015a620
# STATUS: named SataDiskInitialize at 0x8015a408
.set AddonNoTrinMuPatch, 0x800e3520
.set AddonNoWiredLanPatch, 0x800d2ddc
# STATUS: named NicDriverEntry at 0x800d2db8
.set AddonNoCorMu, 0x8008e9d0
.set NicAttachPatch, 0x800d3064
