// Plotting Framework
//
// Copyright (C) 2019-2020  Mario Krüger
// Contact: mario.kruger@cern.ch
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PlotManager_h
#define PlotManager_h

#include "PlottingFramework.h"
#include "PlotPainter.h"
#include "Plot.h"

using namespace PlottingFramework;
namespace PlottingFramework
{
//****************************************************************************************
/**
 * Central manager class.
*/
//****************************************************************************************
class PlotManager
{
public:
  PlotManager();
  virtual ~PlotManager();

  // :: user settings ::
  
  // settings for output
  void SetOutputDirectory(string path);
  void SetUseUniquePlotNames(bool useUniquePlotNames = true){mUseUniquePlotNames = useUniquePlotNames;} // if true plot names are set to plotName_IN_figureGroup[.pdf,...]
  void SetOutputFileName(string fileName = "ResultPlots.root") {mOutputFileName = fileName;} // in case canvases should be saved in .root file

  // settings related to the input root files
  void AddInputDataFiles(string inputIdentifier, vector<string> inputFilePathList);
  void AddInputDataFile(string inputIdentifier, string inputFilePath);
  void DumpInputDataFiles(string configFileName); // save input file paths to config file
  void LoadInputDataFiles(string configFileName); // load the input file paths from config file

  // remove all loaded input data (histograms, graphs, ...) from the manager (usually not needed)
  void ClearLoadedData() {mDataLedger->Delete(); mLoadedData.clear();};

  // add plots or templates for plots to the manager
  void AddPlot(Plot& plot);
  void AddPlotTemplate(Plot& plotTemplate);

  // saving plot definitions to external file (which can e.g. be read by the commandlne plotting app included in the framework)
  void DumpPlots(string plotFileName, string figureGroup = "", vector<string> plotNames = {});
  void DumpPlot(string plotFileName, string figureGroup, string plotName);

  // read plots from plot definition file created by the above functions (regular expressions are allowed);
  // the mode variable can be "load" to add these plots to the manager, or "find" to check only if the specified plots exist (prints out this info)
  void ExtractPlotsFromFile(string plotFileName, vector<string> figureGroupsWithCategoryUser, vector<string> plotNamesUser, string mode = "load");


  // after desired plots were added to the manager they can be created
  // the program then will try to extract the required input data (TH1,TGraph,..) from the specified input files (.root output of your analysis)
  // there are several modes:
  // "interactive": you will be prompted the plot and can scroll through all plots by double clicking on the right side of the canvas
  // "pdf", "png": plots will be stored as such files in the specified output directory (subdirectories are created for the figure groups and categories)
  // "macro": plots are saved as root macros (.C)
  // "file": all plots (canvases) are put in a .root file with a directory structure corresponding to figure groups and categories
  void CreatePlots(string figureGroup = "", string figureCategory = "", vector<string> plotNames = {}, string outputMode = "pdf");
  void CreatePlot(string name, string figureGroup, string figureCategory = "", string outputMode = "pdf");

  
private:
  void ReadDataFromCSVFiles(TObjArray& outputDataArray, vector<string> fileNames, string inputIdentifier);
  void ReadDataFromFiles(TObjArray& outputDataArray, vector<string> fileNames, vector<string> dataNames, vector<string> newDataNames = {});
  void ReadData(TObject* folder, TObjArray& outputDataArray, vector<string>& dataNames, vector<string>& newDataNames);

  
  TObject* FindSubDirectory(TObject* folder, vector<string> subDirs);
  inline int GetNameRegisterID(const string& name);
  inline const string& GetNameRegisterName(int nameID);
  void LoadData(map<int, set<int>>& requiredData);
  void GeneratePlot(Plot& plot, string outputMode = "pdf");
  bool IsPlotPossible(Plot &plot);
  bool IsPlotAlreadyBooked(string plotName){for(auto& plot : mPlots){if(plot.GetUniqueName() == plotName) return true;} return false;};
  ptree& ReadPlotTemplatesFromFile(string& plotFileName);

  TApplication mApp;
  vector<string> splitString(string argString, char deliminator = ':');
  map<string, int> mNameRegister; // bi-directional mapping between name and unique id
  map<int, set<int>> mLoadedData;
  bool mSaveToRootFile;
  string mOutputFileName;
  map<string, shared_ptr<TCanvas>> mPlotLedger;
  string mOutputDirectory;    /// directory to store output
  bool mUseUniquePlotNames; // wether to use plot names or unique plot names for output files
  map<string, vector<string>> mInputFiles; // inputFileIdentifier, inputFilePaths
  TObjArray* mDataLedger;     /// all external data representations currently loaded by the manager
  vector<Plot> mPlots;
  vector<Plot> mPlotTemplates;
  map<string, ptree> mPropertyTreeCache;
  vector<string> mPlotViewHistory;
};

} // end namespace PlottingFramework
#endif /* PlotManager_h */
