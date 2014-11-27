GEMAnalyzer
===========

## Usagae 
``` bash
# please, start with CMSSW workspace.
cmsrel CMSSW_6_2_0_SLHC20_patch1
cd src

# Initialize workspace
git clone git@github.com:geonmo/GEMAnalyzer.git
scram b

# Create geometry information for each geometry version. It needs only 1 event.
cd GEMAnalyzer/digiAnalyzer/test
cmsRun test_v4_cfg.py && cmsRun test_v6_cfg.py && cmsRun test_v7_cfg.py

# Ploting.
python -i checker.py
```

## Explain about checker.py
- Only change "cut" value for plots.
ex) "region == 1 && chamber ==1"
ex) "abs(region)==1 && chamber < 10 && layer == 1"


