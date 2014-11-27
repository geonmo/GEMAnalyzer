#!/usr/bin/env python
from ROOT import *
import copy

def GetMinMax(h1) :
  xaxis = h1.GetXaxis()
  yaxis = h1.GetYaxis()
  xmin, xmax = xaxis.GetXmin(), xaxis.GetXmax();
  ymin, ymax = yaxis.GetXmin(), yaxis.GetXmax();
  print xmin,xmax, ymin,ymax
  return xmin,xmax, ymin,ymax

def PrintStripPhi(nt, cut="1", scale = 40) :
  nt.Draw("strip:phi",cut)
  return

def GetDetPhi( nt, cut="1", scale = 40 ) :
  nt.Draw("int(station-1)*2+int(layer):phi >> temp_detphi",cut)
  h1 = gDirectory.Get("temp_detphi")
  xmin, xmax, ymin, ymax = GetMinMax(h1)
  nbinx = int(xmax-xmin) * scale 
  nbiny = int(ymax-ymin)
  title = "phi vs Detector label with %s; #phi(degree) / %.3f ; # of Strip"%( cut,  (xmax-xmin)/nbinx )
  h1 = TH2F("phi_det",title,nbinx,xmin,xmax, 6, 1, 7)
  yaxis = h1.GetYaxis()
  yaxis.SetBinLabel(1,"St1,La1")
  yaxis.SetBinLabel(2,"St1,La2")
  yaxis.SetBinLabel(3,"St2,La1")
  yaxis.SetBinLabel(4,"St2,La2")
  yaxis.SetBinLabel(5,"St3,La1")
  yaxis.SetBinLabel(6,"St3,La2")
  nt.Draw("int(station-1)*2+int(layer):phi >> phi_det",cut)
  h1.SetStats(0)
  return h1
   

def GetStripPhi(nt, cut="1", scale = 40 ) :
  nt.Draw("strip:phi >> temp_hist",cut)
  h1 = gDirectory.Get("temp_hist")
  xmin, xmax, ymin, ymax = GetMinMax(h1)
  print xmin, xmax, ymin, ymax
  nbinx = int(xmax-xmin) * scale
  nbiny = int(ymax-ymin)
  title = "phi vs strip ; #phi(degree) / %.3f ; # of Strip"%((xmax-xmin)/nbinx)
  h1 = TH2F("phi_strip",title,nbinx,xmin,xmax, nbiny, ymin, ymax)
  nt.Draw("strip:phi >> phi_strip",cut)
  print xmin,xmax, h1.GetXaxis().GetBinWidth(1)
  return h1

  
  
if __name__ == '__main__' :
  file = TFile("ntuple_geom_v4.root")
  nt = file.Get("demo/strip")

  cut = "abs(phi)<10 && region == 1 && (chamber ==1)"

  c1 = TCanvas("c1","c1",3600,1800)
  c1.Divide(2,1)
  c1.cd(1)
  h1 = GetStripPhi( nt, cut)
  h1.Draw("colz")
  c1.cd(2)
  h2 = GetDetPhi(nt, cut)
  h2.Draw("colz") 
  c1.Update()
  c1.SaveAs("c1.png")

