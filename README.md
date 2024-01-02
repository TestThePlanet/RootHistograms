The histogram's gradient is set in makeAllPlots.C starting line 277

TODO: 
- [x] Convert to struct wrapper.
- [x] Get rid of the blue histogram outlines, see line 459 PrettyHist
- [ ] Use Gradients
- [ ] Make white not actually be white. See newTCanvas in CMSStyle.C
    suggest using root color 10 (RGB=254)
- [ ] Make a dark mode
    bkg color: TColor::GetColor(0.188f, 0.22f, 0.255f)
    text color: TColor::GetColor(0.847f, 0.871f, 0.914f)
- [ ] Make a strip mode for webpage integration
- [x] Guard against '/' in histogram names.
