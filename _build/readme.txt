xeBuild 1.22
============


Introduction:
=============
    xeBuild is a command line system image builder for JTAG, glitch, and clean images.
    Run the xeBuild program with no (or incorrect) arguments to see it's usage info.

What's New:
===========
    - bug fixes
	- add 17502
    - !experimental! g2 and g2m patches based on 13182 CBB for coronas using winbond memory
      patch set for corona hardware issues that require 13182, use -r WB or -r WB4G on command line to select when building corona images
      thanks to 15432 and DrSchottky for doing the heavy lifting for corona winbond patches
      original release can be found http://www.hackfaq.net/main/xell_wb2k/
      Mileage may vary, though it's possible these can also improve boot times of older corona models and possibly even some trinity machines (use trinity smc).
      Also, it's unlikely vfuses will work fully on machines that have vfuse errors as mfg CBB patches never load them from NAND (CD onwards do still.)
      Big thanks also to Team Xecuter for first bringing this problem to our attention, and working out and testing the solution that proved it by successfully booting xell

Current Limitations:
====================
    - STAY THE HELL OFF LIVE! Nuff said, we're not you're mum.

How To Use:
===========
    - See individual folders for lists of files to provide
    - if desired provide replacement cpu and 1bl keys in text files
    - open a command window in the xeBuild directory
    - on the command line type, for example:

    example - if you provided keys in appropriate text files

        xeBuild.exe -t glitch -c falcon -d myfalcon myfalconout.bin

        -t glitch = build a glitch type image
        -c falcon = use falcon bl and patch set
        -d myfalcon = a folder is present called "myfalcon" with per machine files, this uses it
        myfalconout.bin = the file that will be produced

    - type 'xeBuild.exe -?', 'xebuild client -?' or 'xebuild update -?' for command line info

Update and Client modes:
========================
    Both modes require the supported updsvr running on the xbox, full functionality may require
    updating console patches with the included hv patches. Both the PC and the xbox need to be on
    the same subnet/LAN router.
    
    Client mode is a simple way to read, write and patch flash as well as few other simple commands
    such as the patch updater. The patch updater will look in the folders beside the exe for
    {version#}\bin\patches_{type}.bin
    which are full patches for whichever console and hack type, it will load and strip the patches
    if needed and send them to the console. Note that only xebuild images are truly supported for this.
    Most of the client mode commands should be available on any console, even unhacked devkits. See output
    from 'xebuild client -?' for more information on the options available.

    Update mode attempts to retain as much data about the console as possible, without having to
    provide any info on the command line aside from optional/addon patches if required. After you
    copy the $SystemUpdate folder into (in this example) the folder 16203 it is capable of taking
    a simple command line like:
    xebuild update -f 16203 -a nohdmiwait
    It will fetch all the info from the console, and use the updater to update both the system flash
    and avatar data on the console (provided you have an 360 formatted HDD internally in the console.)
    It has some more advanced options to allow one to build the update image as well as dump the data
    from the console as it's acquired, while even leaving the console data untouched. See output
    from 'xebuild update -?' for more information on the options available.
    
    Neither update or client image writes are able to affect bad blocks, but are able to write new ones.
    If this happens mistakenly, an erase block command has been provided in client that will attempt to
    clear the bad block - use with caution though, blocks get marked as bad for good reasons and is a normal
    occurrence on NAND when a block becomes unreliable.
    
    With big block machines, the server will attempt to retain any NAND mu data in the system area, provided
    there is no system data to write in the image being sent. It's not foolproof, but update mode should not
    corrupt NAND mu.

Example:
========
    -take original console dump, put it in mytrinity folder as nanddump.bin
    -set CPU key and 1BL key in ini file, verify LDV from nanddump.bin matches console fuses
      if not set cfldv in ini file
    -build (xeBuild.exe -t glitch -d mytrinity -f 13599), flash and hopefully life is good

.ini files:
===========
    Just a word on the format... the ini parser is not very robust, the files need
    to be plain ASCII, everything after a ; on a line is ignored, and spaces are
    not acceptable (they get removed).

    Things like CPU key and 1BL key, if present in the per box ini file need not be
    placed anywhere else.

Optional Patches:
=================
    Various optional patches are included for use with the -a option, they are:
    nofcrt     - removes fcrt.bin requirement on some drives
    nohdd      - disables detection of internal SATA HDD
    noSShdd    - disables internal SATA HDD with valid retail security sector
    nohdmiwait - HDMI consoles will no longer wait or EXX screen when video is not ready
    nolan      - disables wired LAN to prevent E75/76/77 on machines with a damaged PHY
    nointmu    - disables jasper nandmu, trinity 4G internal USB and corona 4G MMC memory units
    nowifi     - disables usb wifi adapters including the ones built into 360S/E

blmod.bin:
==========
    Changing the patches to the BL that follows the BL that is executing during glitch attempts
    has a direct effect on whether a machine will glitch. The provided patches are generic
    and work well on most machines, but this per machine build addon can now be supplied without
    modifying the base patches to CBB or CD via a file in the perbuild folder, they will simply be
    tacked onto the end of CBB or CD, and the BL size adjusted to include this new data in the hash.

    Keep in mind, it can take multiple attempts and re-flashing with different binary data to find
    something that will boot at all, let alone be more effective for your console.
    
    blmod is currently not supported by update mode.

Note:
=====
    - DON'T USE THIS UNLESS YOU KNOW FOR SURE THAT YOU NEED IT! Using an incorrect
    controller config can result in problems remapping bad blocks (even manually.)
    If you have a 16M jasper, an additional build type has been added
    'jaspersb', by default the image will be built for jasper with big block
    controller (config 00023010), use this alternate switch to build for small
    block controller (config 01198010.)

Multi build/options example:
============================
    when you specify -f 13599 on the command line:
        13599\filelist.ini
    is parsed instead of data\filelist.ini

    Also the bin directory is used from
        13599\bin\
    instead of
        bin\
    allowing anyone to create multiple builds without multiple instances or
    rebuilds/hex edits/hacks of the main app.

    The example provided is the last version of 13599 patch set from dash launch and
        other files to build freeboot 13599

    example use:
    ------------
    xeBuild -f 13599 -d myfalcon x13599out.bin

    -f 13599     : use .\13599\filelist.ini, and .\13599\ for firmware files, .\13599\bin\ for patches
    -d myfalcon  : use .\myfalcon for per build files (cpu key, keyvault, security files, ini etc.)
    x13599out.bin: override auto generated name and produce .\x13599out.bin as the final NAND image

    note, if -d ***** is not specified it will still use the original /data and /bin dirs

Devkit image building:
======================
    This feature is currently considered Beta/Work In Progress.

    A new image target type was added, "-t devkit" which builds 64M flash images for devkits. Currently untested,
    building with a 00 filled CPU key will create a zeropaired devkit image that may allow one to boot a software
    bricked devkit that one does not know the CPU key for and recover it to an operational state. By powering on
    the console with such an image present, with a recovery DVD in the drive, the recovery software should be able
    to create a new keyvault, re-pair the DVD drive to the new keyvault, and allow normal operation once complete.
    
    Normal devkit image building when one does know their CPU key and thus has security files and keyvault should
    work as expected.
    
    Building devkit for glitch/jtag is also possible using the standard -t glitch/jtag methods. Sample ini
    have been provided with this release, but will not work unless patches and files are supplied. Note that devkit
    is not our focus, but was relatively easy and straight forward option to supply for those that wish to make
    use of it.

jasperbigffs:
=============
    Those who use large block NAND are now able to nearly double the size of the system file area
    with this option with no apparent ill effects. Normally this option wouldn't be needed, but if one
    wanted to experiment with more files in flash, or one was building a devkit image for a devkit with
    a big block flash, this option is required.

[rawpatch]:
===========
    It is now possible to force a raw patch of NAND image via files and offset both through the command line
    and via the data directory ini files. This causes a file to be read and byte copied into the raw flash image
    before spare ecd/ecc is calculated at the supplied offset in flash (without spare).
    
    Note that both the ini entries and the command line will interpret the offset as decimal if not preceded by '0x'.
    There will be no spare marks or file system entries created for such raw patches, it is user beware and up to
    anyone who needs this to ensure the data is being placed somewhere safe.
    
    No example is given in the supplied ini files, the segment can be placed anywhere above [flashfs] and would
    look something like this:
[rawpatch]
somefile.ext,0x123456; this offset is interpreted as hexadecimal
somefile2.bin,123456; this offset is interpreted as decimal


support:
========
    If you've found a bug or have a suggestion, please comment at
    http://www.realmodscene.com/index.php?/forum/15-xebuild/ (English)
    http://homebrew-connection.org/forum/index.php?board=8.0 (English/French)

Credits:
========
    Without ikari this would not have been possible, thanks!
          __               ____   ___   ___ _____
         / _|_ __ ___  ___| __ ) / _ \ / _ \_   _|
        | |_| '__/ _ \/ _ \  _ \| | | | | | || |
        |  _| | |  __/  __/ |_) | |_| | |_| || |
        |_| |_|  \___|\___|____/ \___/ \___/ |_|
                     [v0.10 - inspired by ikari]
        R.I.P.
    No this isn't freeboot, it is a clone and has always been since the last
    release of ibuild.

    Thanks and greetz to everyone who has contributed to hacking this
    wonderful machine. Thanks to the engineers and countless others who made
    the machine what it is... we only wish they had listened and RROD was
    not a problem. If we were to list everyone here, there would be no time
    left to play on the machine!

    Thanks Team Xecuter for the Corona 4G! Thanks to JuggaHax, dayton360mods,
    glitch360team and all other contributors for helping find a way to make Corona 4G golden!

    Thanks to Free60, LibXenon.org, Redline99 and Tuxuser for providing xell builds <3

    Thanks to Swizzy for making the official GUI front end for xeBuild, for always
    adding the new stuff we shovel at him and never once complaining.
    
    Big thanks to the folks at #freeboot on efnet for the tireless
    hours of help you all give freely. Thanks to the testers who tirelessly
    made sure stuff worked. Thanks to rgloader for doing the work yourselves,
    there *is* no spoon, just a glitch in the matrix.

    Don't believe what random people *cough* write on forums ..

-----
//2021
-----

Changes:
========
1.22
- fixed an inconsistency with <d> not needing a trailing \ in the "client -e" option, thanks spk!
- added proper devkit SF/SG encryption, thanks xvm!
- improved CB/SB decryption check when looking for pairing data from a dump
- fixed longstanding typo that refered to CF/SF as zeropaired when it was not a JTAG image

1.21
- add 17559

1.20
- add 17544

1.19.1
- nowifi addon patches replaced with ones that should actually work (thanks Jester!)

1.19
- add 17526

1.18
- add 17511

1.17
- fix a long standing bug when zerofilling bad blocks within the reserve block area (thanks GoJohnny!)
- add 17502
- many internal changes were made to make the code more portable, if you run into a major issue this is probably why
    please report it, an immediate solution should be to use the previous version's exe

1.16
- fix missing fsroot offset correction on big block machines (thanks BugReporter, mik30, nexus!)
- add additional diagnostic output to mobile data detection when -v2 is specified
- document dummy 'extract' mode which currently only performs dump loading and verification for diagnostics
- account for 1 page mobile*.dat delete entries
- show all fuses with client -i
- fix mmc only detecting fsroot when scanning for mobile*.dat (thanks liquidzorch!)
- now possible to specify external/optional/addon patches in perbuild ini file
- add 17489

1.15
- add additional checks to sort big block and mmc overdumps better
- retain netKd info from header if present along with mobiles
- 15574+ patch added to allow data dvd with 360 game to show name/tile info in official dash
- added command line option -8 and [rawpatch] segment to data dir ini, can have up to 16 file slots with offset
- added 'testkit' and 'testkit16' targets, same as devkit but has 2 keyvaults
- fixed a problem with skipping newer versions of small/single page mobile files on small block NAND
- trim addon patches if a tailing 0xFFFFFFFF was mistakenly included (thanks sk!)
- fsroot scan now collects info on mobiles for later extraction
- fix bugs with locating/extracting small mobile files in some rare cases
- add 17349

1.14
- add patch (16767+) to allow xbox 360 formatted HDD without security sector to work with transfer cable (thanks Mustache!)
    (note: xfer cable supported by aurora and F3 file managers)
- fix ini file not including launch.ini
- fixed an issue with detecting/revoking devkit boot chains in update mode
- add 17148, 17150

1.13
- add 16756, 16767
- add 13182 based corona configuration to retail ini
- fix a bug that froze file times to a default value for flash files
- update kernel patch for XeKeysConsoleSignatureVerification, content signed with a different keyvault (remote signed) will now reply as locally signed (v16747+)
- added additional trinity/winchester targets in case they are needed
- experimental patch set for corona hardware issues that require 13182 (16767+) (thanks 15432 and DrSchottky!)
- reworked how update mode works on glitch images, it now retains the bootloaders from the console instead of rebuilding from files (making it simpler to update the new corona patches in the future)
- added noSShdd addon patch for 16767+, with this patch enabled retail drives with a valid security sector will be rejected

1.12
- check FCRT.bin signature with PIRS_pub.bin or MAST_pub.bin if available (selection based on content)
- check DAEP signed signatures in DAE.bin (usually 2) with PIRS_pub.bin if available
- check CRLP signed CRL.bin signatures with PIRS_pub.bin if available
- fix mobile extraction stalling process on corrupt NAND
- do not patch boot reasons into flash header for devkit and retail builds, only glitch and jtag
- added patch to kernel to attempt to block network until launch.xex has loaded (if available)
- add -o smcnocheck to image build options/ini; avoids fatal build error if smc is unknown
- add 16747
- fixed: was not automatically creating all the folders for 16747 avatar data to be valid

1.11
- fix a bug which caused an infinite loop when a mobile*.dat was smaller than 512 bytes - Foo you too mobileE.dat! (thanks blakcat!)

1.10
- add 16547
- fix a bug with build mode mobile*.dat extraction when multiples of the same settings are stored in the same block but version number does not increase
- minor fixes

1.09
- now checks devkit BL (SB/SC/SD/SE) signatures/built in hashes while building devkit images
- adds client -i <d> method (now with less bugs, thanks blaKCat!)
- client -i wasn't showing cpu/dvd key (thanks blaKCat!)
- fix bug in freeboot (JTAG) core that caused options and poweron reason to be ignored (thanks siz and swizzy!)

1.08
- align patch slots properly on glitch images
- add ability to make zero-paired dual cb images (retail/glitch)
- unknown devkit smcs were not being checked properly and always reported hacked
- added support for 'blmod.bin' per-build file, can assist fine tuning glitch machines that don't play well with standard patches
- added CPU key corruption checks
- added a secondary check on nanddump.bin size after determining big block/small block
- added new build target, glitch2m, which uses mfg cba to boot with virtual fuses (only trinity/corona, 16203+)
- added nolan addon patch for 16197+ for those with bad wired lan phy getting E75/76/77; should not affect wireless dongles
- consolidate internal memory unit disable patches into nointmu addon, Corona 4G internal memory can now be disabled (16197+)
- added integrity checks for: blocks that appear to be remapped but are outside the remap area; CF slot size
- updated all patch sets to use hv built in memcpy (better peek support for things that require 64bit reads like 1bl ROM)
- check for SU container in version\$SystemUpdate subfolder as well as version folder
- nonces will attempt to be kept when providing nanddump.bin (some claim this affects glitch boot times, makes differential flashing more effective)
- Xstress.settings has been renamed to Manufacturing.data
- update jtag freeboot core to V0.9 to use options stored in flash header instead of patched directly into the binary
- added update and client mode
- -d options no longer need to be relative to the exe launch path
- checks keyvault signature (if present) when MAST_pub.bin is available from update server or file in .\ or .\common\
- checks CB/CBA signature when 1BL_pub.bin is available from update server or file in .\ or .\common\
- updated xell builds to XeLL_Reloaded-2stages-v0.993
- add 16537

1.07
- trinitybigffs added (big block devkit sized file system for trinity)
- -s option added to create a .sha1 file along with the final image
    if no arg is provided, the final image name will be used with .sha1 appended as extension
- change behaviour of patchsmc
- add 16203

1.06
- fix an issue with identifying FCRT console from keyvault (thanks stefanou!)
- corrected some status message typos
- added sanity check to bl patcher
- added (starting with 16197) fcrt signature check patch (supports swapping drive + kv/dvdkey + fcrt on drives/consoles where nofcrt does not enable game disks)
- added 16202
- correct 16197/16202 nohdd patch to disable only the hdd, not every SATA device

1.05
- tidy ini_creator output a little
- colons were not being removed from macid string in ini file, fixed
- added corona4g build target and dump parsing, nanddump.bin should be minimum 0x3000000 bytes from corona mmc machines (no spare data)
- Xstress.settings will now be kept along with other mobile data
- added flash header sanity checks
- adjust dump loader to allow for 64bit file sizes
- build process will no longer complete a retail image if smc is patched or a jtag image if smc is unpatched. Apparently a big fat error warning isn't enough.
- added some older retail.ini, added new switch to allow selecting specific bl configs (if making images with patches, the switch is appended to patch names as well)
- add 16197

1.04
- blacklist sysupdate.xex* as flashfs files, these are auto generated files representing data that overflows the patch slot
- add console ID, motherboard serial number, serial number and mfg date to final output (when available)
- add in corona support files for 15574 and 14719, thanks again to Team Xecutor's RGH crew for
    providing the exploitable BLs!

1.03
- updated patches to remove CON sig checks, still allowing the patched check to report if the CON was signed by this machine
- updated glitch patches to remove CF LDV check (keep in mind updating a fat to 14717+ requires rewiring CPLD for less stable glitching)
- modified [flashfs] category
    - can now take longer paths as well as absolute drive paths, spaces not allowed (ie: ..\common\filename or H:\somepath\filename)
    - items without a crc will not be sought outside the given path or filename (relative paths are based in the firmware folder, xexp filename mutations will NOT be applied)
    - items with a crc will be sought in given path, system update, nanddump.bin then common folder
- fix 16M corona smc extraction from nanddump.bin
- correct nandmu option so it properly defaults to false
- add 'jasperbb' console target (same as jasper256 and jasper512)
- correct bug in hv patches in 14717/14719
- added smcnoeject and smcnoblink options (only patches jtag/glitch smcs)
- changed glitch image CD patches to not require dynamic patches (should be more stable)
- add 'demon' option, currently only sets the same speed as cygnos uart speed
- add 15574

1.02
- improved feedback when mangled or incorrect option values are found in options.ini or command line
- fix bad LBA due to using a small block controller flash image on a big block machine
- added patch to all versions to skip yet another minimum version check (mostly affected default.xex on root of USB)
- add optional nohdmiwait patch to 14717/14719 (console won't pause bootup waiting for HDCP handshake when TV doesn't respond)
    known side effect: occasionally when the TV does finally sync dash will restart (forced to metro even if using dash launch)
- now retains Statistics.settings from a nanddump.bin and can load the data from perbuild dir along with other mobile data
    this data is found in the block preceding smc_config
- can now obtain CF/CG and flash files from su20076000_00000000 (system updater container) when placed in firmware folder
   - bls besides CF/CG must still be provided externally
   - .xex/.xtt files that only have update/.xexp in the container still need to be provided externally
   - new option 'nosusecurity' added to command line and ini to skip using security files from system update container,
       external files provided in perbuild directory take precedence over any other security files found (order: file, su, dump)
- now attempts to retrieve files not found in firmware folder or update container from nanddump (if provided)
- common folder added to scan path for alternate bootloader location
- now respects setting bool options on command line to false instead of enable only, and overrides/ignores enable options set elsewhere
- fixed regression around remapping blocks when wear area has bad blocks
- jtag uses second CB to enumerate fuse values, displays virtual fuse set at end of bl encoding stage in verbose logs
- revoke nanddump.bin that has had zeropair data overwritten and big block images that had bootloader data overwritten with incorrect nandpro args
- fixes for big block retail images (patch slot offset and reserve blocks value)
- BIG thanks to Team Xecuter's RGH crew for snagging fat dual CBs
- add glitch2 build type, uses console type as base for patch file names (ie: patches_g2falcon.bin)
- add 'notrinmu' optional patch for 14717/14719 to disable trinity internal 4G memory unit access
- add 'nohdd' optional patch for 14717/14719 to disable internal hard drive access
- fixed a bug relating to relative paths

1.01 update pack 1
- fixes a bug with ini creator, wasn't outputting non CB/CD bl data
- add 14719
1.01
- minor bugs fixed (extended.bin, kiosk button not displaying)
- invalid secdata.bin and extended.bin will be cleanly recreated instead of failing build
- can now accept decrypted kv.bin without messing it up
- fixed a bug with long version strings in firmware .ini files
- fixed fatal exception when patch file is not found
- added -i flag to specify additional addon component for ini/patch file name
- corrected nandmu warning to only say xebuild will attempt to keep this data when the option is set
- added additional jasper build mode jasperbigffs, results in a non-standard and much larger system file area (approx 32MiB larger)
- patch slot address for glitch/retail images is now dynamically assigned (first block after xell/first block after CE)
- devkit image building added
- if pairing value can't be found in dump CF/CG it will attempt to be extracted from CB
- smc size and address made dynamic (mainly for corona+)
- corrected typo/problem with FAT bitmap creation
- cache decrypted keyvault, refine messages regarding FCRT and output dvdkey at the end
- logs/outputs expected/possible fuse values for console sequence bytes in CB
- add dvdkey to ini and -o, to set dvd key in keyvault before writing it to the new image
- fixed a possibly critical bug when parsing nanddump.bin FS entry
- correct EU/AUS smc game region output
- nanddump flash controller detect recoded, now only requires block 0 be not remapped
- fixed unhandled exception when -o option that requires = did not have =
- updated bl patches for all jtag machines and trinity (rgh fat doesn't need) to remove smc size = 0x3000 limit
- fixed bug that was causing 2nd patch slot on retail builds to contain unneeded data
- added fuse mask output while processing CB
- added 14717
- added patch to trinity 9188 CB_B to bypass fuseline 2 revocation check

1.00hf
- hotfix - jtag images were being created with incorrect patch file number (xexp1 instead of xexp2)
1.00
- gets security files from nanddump.bin and verifies them (odd.bin is currently not processed)
- option added to disable extracting security files from nanddump.bin
- decrypts perbuild security files for verification (crl/dae only currently, updater files work too)
- zero nonce data in bls before checking crc (included file lists updated with new crc and explanation)
- fixed a bug with mobile extraction
- fixed a bug with fsroot processing
- (glitch) dynamic SMC patcher, no longer limited to hard coded hash/offsets
- added more SMC hashes to verify known clean SMCs
- will attempt to decrypt external encrypted smc.bin if needed
- whitelist more chars in the file list parser
- altered so that pairing value will be retrieved from nanddump.bin even if ldv is set in ini
- dual CB is dictated by ini, "none" filename indicates single CB (jtag does not use dual CB)
- increased logged info when adding files to flashfs
- odd.bin in encrypted (only!) form is now handled (from file or nanddump.bin)
- ini options are now available as -o options on command line
- added -t command line flag for glitch/retail/jtag selection
- JTAG image creation merged
- separate retail/glitch/jtag into individual per-firmware ini lists
- added -noenter command line option to suppress application asking to press enter on completion
- added proper errorlevel exiting, 1=usage/commandline error, 2=file write err, 3=image build error
- add 'cygnos' and 'xellbutton' options for glitch images with appropriate bl patches (either may affect fat glitch boot rate!)
- non-critical spare data fix to the way smc config is added to image
- update freeboot core and glitch base patches to accept a secondary xell poweron reason
- rewrote extended.bin handler, given an invalid/undecryptable file it will create a empty extended.bin
- rewrote keyvault handler, can decrypt and verify kv.bin when it's provided encrypted
- added patch append -a command, and converted nomu and nofcrt to optional patches
- added simple explanation of patch file formats in about_patches.S
- changed nonandmu option to nandmu so it can default to false
- added corona and winchester console types, currently not supported but there if needed
- add 14699

0.33
- corrected bug with ini parsing and dvd region (and others) left blank
- add 13604

0.32
- slim/fat glitch image building (based on fbbuild 0.32)
- builds retail images with -retail command line option
- added autopatch smc option in per box ini file
- extracts pairing value and highest LDV from nanddump.bin
    (ini cfldv setting overrides nanddump ldv)
