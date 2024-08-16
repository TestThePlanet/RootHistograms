 - [ ] fix COMMANDS LIST.txt
 - [ ] replace google drive

 - [x] Append unique contributers text
 - [x] Append integral text
 - [x] Put a time stamp in the upper left corner.
 - [x] Make a setup script to get Ilya back into git with aliases.
 - [ ] Test setup script. 
 - [x] Make Ilya a script to do everything in one step. 
 - [x] Modify makeOverlays to add a second plot
 - [x] Automatic downloader
 - [x] Fix known bugs
 - [x] Move HM to bottom of the screen
 - [x] Contributers off by 1?
 - [x] need to implement single plot mode in the overlays.
 - [x] make sure make2Overlays does a remove of \_final/ when appropriate
 - [x] For speed, make plots locally, then copy them to google drive.
 - [ ] Make a log of some kind.
 - [x] Move copy to google drive from main to copyToGoogleDrive.py, as a stand-alone utility.

 - [x] Points System
From https://www.testtheplanet.org/interop-points
50 point pack "Full House" (2XCrash2.5 + 2X OSHA_withGrimaceStored)
25 point pack criteria (2XC2.5 + 1 OSHA_withGrimaceStored)
20 point pack criteria (2xC2.5)
15 point pack criteria (1XC2.5 + 2xOSHA_withGrimaceStored)
12 point - 1XCrash2.5 and 1XOSHA_withGrimaceStored 
10 point pack criteria (2xOSHA_withGrimaceStored)
5 points (1xCrash2.5)
2 points (1xOSHA_withGrimaceStored)
0.6 points (1x Crash2.5short)

To get combo points, tests have to be on the same mask, by same person, 
in the same initial configuration(no tension adjustment for disposables). 

Once you get a Full House combo pack for a mask model, the points for each additional test follow non-combo rules again.
(5 for each C2.5 and 2 for each OSHA.)

- [ ] Score explanation for each user.

- [ ] Supplimental Scoring 
Unconnected sample port to ambient port check - 15pts per start of session entry(same date entry) (only if  
at least one respirator is tested the same day - you can't rack up 
points just by running the machine unconnected) 
Controls Mask Low Bound(such as R1S1) - Crash2.5 - 15 points for one test
Controls Mask Medium bound(such as U5Y9) - Crash2.5 - 15 points points for one test
Zero Check MAX FF check - 6 minutes - 20 points
Post test session UNCONNECTED - 30 points
2024-04-19 - Interoperability points system posted
2024-04-29 - extra 100 point bonus for testing rare, historical respirators that have FTtP ID numbers. 
2024-04-29 - doff/redon instruction added for combo packs to be more interoperable with Newcomb 2007

- [ ] Google drive mount
    sudo mount -t drvfs G: /mnt/g
    need sudo
