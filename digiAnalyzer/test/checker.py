#!/usr/bin/env python
from ROOT import *
from geometryChecker import *

filev4 = TFile("ntuple_geom_v4.root")
ntuple_v4 = filev4.Get("demo/strip")
filev6 = TFile("ntuple_geom_v6.root")
ntuple_v6 = filev6.Get("demo/strip")
filev7 = TFile("ntuple_geom_v7.root")
ntuple_v7 = filev7.Get("demo/strip")

cut = "region == 1 && chamber ==1"

c1 = TCanvas("gc","GeometryChecker",3600,2400)
c1.Divide(3,2)
#c1.Draw()

c1.cd(1)
h1 = GetStripPhi( ntuple_v4, "v4", cut)
h1.Draw("colz")

c1.cd(4)
h2 = GetDetPhi( ntuple_v4, "v4", cut)
h2.Draw("colz")


c1.cd(2)
h3 = GetStripPhi( ntuple_v6, "v6", cut)
h3.Draw("colz")

c1.cd(5)
h4 = GetDetPhi( ntuple_v6, "v6", cut)
h4.Draw("colz")

c1.cd(3)
h5 = GetStripPhi( ntuple_v7, "v7", cut)
h5.Draw("colz")

c1.cd(6)
h6 = GetDetPhi( ntuple_v7, "v7", cut)
h6.Draw("colz")
#c1.Update()
c1.SaveAs("plot.png")