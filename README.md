The histogram's gradient is set in makeAllPlots.C starting line 277

TODO: 
- [x] Convert to struct wrapper.
- [x] Get rid of the blue histogram outlines, see line 459 PrettyHist
- [x] Use Gradients
    See any that you like? https://root.cern/doc/master/classTColor.html#C06
- [x] Tune Gradients
- [x] Refrase color bars for low N unique testers, using testerID.
- [x] Display N unique testers
- [x] Display sample N
- [x] Make white not actually be white. See newTCanvas in CMSStyle.C
- [ ] Make a dark mode that doesn't look terrible
    bkg color: TColor::GetColor(0.188f, 0.22f, 0.255f)
    text color: TColor::GetColor(0.847f, 0.871f, 0.914f)
- [ ] Make a strip mode for webpage integration
- [x] Guard against '/' in histogram names.
- [ ] Turn settings into a json or TOML config file to avoid compilation time.
- [x] Add TArrow to harmonic mean
- [x] Add datetime stamp
- [ ] history sequences for animation
- [x] making the option for the output files have a zero padded six digit prefix of the hmff of the whole dataset.
- [ ] Add on the scoring system
- [ ] Improve setup:
    * Install pip 
    * sudo apt-get -y install eog, or other imviewer..
    * install tkinter
    * image magick
