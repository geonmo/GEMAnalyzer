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

def GetDetPhi( nt, desc, cut="1", scale = 40 ) :
  nt.Draw("int(station-1)*2+int(layer):phi >> temp_detphi",cut)
  h1 = gDirectory.Get("temp_detphi")
  xmin, xmax, ymin, ymax = GetMinMax(h1)
  nbinx = int(xmax-xmin) * scale 
  nbiny = int(ymax-ymin)
  title = "phi vs Detector label with %s; #phi(degree) / %.3f ; # of Strip"%( cut,  (xmax-xmin)/nbinx )
  h1 = TH2F("phi_det_%s"%desc,title,nbinx,xmin,xmax, 12, 1, 13)
  yaxis = h1.GetYaxis()
  yaxis.SetBinLabel(1,"St1,La1_even")
  yaxis.SetBinLabel(2,"St1,La1_odd")
  yaxis.SetBinLabel(3,"St1,La2_even")
  yaxis.SetBinLabel(4,"St1,La2_odd")
  yaxis.SetBinLabel(5,"St2,La1_even")
  yaxis.SetBinLabel(6,"St2,La1_odd")
  yaxis.SetBinLabel(7,"St2,La2_even")
  yaxis.SetBinLabel(8,"St2,La2_odd")
  yaxis.SetBinLabel(9,"St3,La1_even")
  yaxis.SetBinLabel(10,"St3,La1_odd")
  yaxis.SetBinLabel(11,"St3,La2_even")
  yaxis.SetBinLabel(12,"St3,La2_odd")
  yaxis.SetLabelSize(0.02)
  yaxis.SetLabelOffset(0)
  draw_option = "int(station-1)*4+int(layer-1)*2+int(chamber%%2)+1:phi >> phi_det_%s"%desc
  nt.Draw(draw_option,cut)
  h1.SetStats(0)
  return h1
   

def GetStripPhi(nt, desc, cut="1", scale = 40 ) :
  nt.Draw("strip:phi >> temp_hist",cut)
  h1 = gDirectory.Get("temp_hist")
  xmin, xmax, ymin, ymax = GetMinMax(h1)
  print xmin, xmax, ymin, ymax
  nbinx = int(xmax-xmin) * scale
  nbiny = int(ymax-ymin)
  title = "phi vs strip ; #phi(degree) / %.3f ; # of Strip"%((xmax-xmin)/nbinx)
  h1 = TH2F("phi_strip_%s"%desc,title,nbinx,xmin,xmax, nbiny, ymin, ymax)
  draw_option = "strip:phi >> phi_strip_%s"%desc
  nt.Draw(draw_option,cut)
  print xmin,xmax, h1.GetXaxis().GetBinWidth(1)
  return h1

  
  
if __name__ == '__main__' :
  file = TFile("ntuple_geom_v4.root")
  nt = file.Get("demo/strip")

  cut = "region == 1 && (chamber ==2)"

  c1 = TCanvas("c1","c1",3600,1800)
  c1.Divide(2,1)
  c1.cd(1)
  h1 = GetStripPhi( nt, "st1", cut)
  h1.Draw("colz")
  c1.cd(2)
  h2 = GetDetPhi(nt, "st1", cut)
  h2.Draw("colz") 
  c1.Update()
  c1.SaveAs("c1.png")

