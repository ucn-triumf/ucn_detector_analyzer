# UCN DETECTOR ANALYZER

Interface with MIDAS for UCN data analysis.

Note: This README written by D. Fujimoto, who did not write the original code set.

## Top-Level Codes

The top-level codes are those found in the top-level of `./src`. These produce executables which are callable by the user.

The top-level codes are defined as follows

* [`midas2root`]: Convert `run*.mid.gz` files to `ucn_run_*.root` files.
* [`anaDisplay`](src/anaDisplay.cxx): Run analysis and produce a display showing the results in a popup window
* [`ana`](src/ana.cxx): Seems to do the same things as [`midas2root`], but saves to `ucn_analyzer_output*.root`. Output file seems corrupted. No header tree.
* [`analyzer_persist`](src/analyzer_persist.cxx): Same as [`midas2root`] but allows histograms to persist across runs.
* [`data_quality_checker`](src/data_quality_checker.cxx): Do some basic data quality checks
  * Look for timing markers at start and end of run
  * Check that pedestal look okay

Adding a new top-level script requires modification to the Makefile. These are all based on the [`TRootanaEventLoop`] class which interfaces well with MIDAS. These objects have the following basic structure:

* [`Initialize()`]: Called after the arguments are processes but before reading the first event is read
* [`Finalize()`]: Called after the last event has been processed, but before any open output files are closed.
* [`BeginRun()`]: Called before the first event of a file is read, but you should prefer Initialize() for general initialization. This method will be called once for each input file.
* [`EndRun()`]: Called after the last event of a file is read, but you should prefer Finalize() for general finalization. This method will be called once for each input file.
* [`ProcessMidasEvent(TDataContainer& dataContainer)`]: The main method, called for each event. Users must implement this function!

These programs must also define a `main()` function, which as a template can be shown below:

```c++
int main(int argc, char *argv[]){
    Analyzer::CreateSingleton<Analyzer>();
    return Analyzer::Get().ExecuteLoop(argc, argv);
}
```

## Module Descriptions

The source code is organized into modules which are roughly defined as follows:

* `Detector`: related to detector data, pulse shape analysis
* `Exp`: cycle parameters and chronobox
* `Misc`: Analysis manager and viewer
* `TUCNTree`: formerly `TUCNTreeMaker.cxx`, split into separate files for clarity of use
* `TV`: DAQ histograms and waveforms (TV792. TV1720, TV1725)
* `Ignore`: filese omitted from Makefile

Adding codes in each module doesn't require edits to the Makefile, the Makefile scans the contents automatically.

## How-To

### Add MIDAS Equipment

For EPICS equipment, adding a few new lines to [`TAnaManager.cxx`](src/Misc/TAnaManager.cxx) is all that's needed. Add a new item to `fEPICSTrees` in the following way:

```c++
new TUCNEpicsTree(odb,
                "EPSR",         // bank name
                "SourceEpics",  // tree name
                "/Equipment/SourceEpics/Settings/Names"), // odb path to branch names
```

then run make in the top-level directory to recompile.

For other equipments, one needs to define a new tree type in a new file, for example [`TSequencerTree.cxx`](src/Misc/TSequencerTree.cxx) and [`TSequencerTree.hxx`](src/Misc/TSequencerTree.hxx), then update [`TAnaManager.cxx`](src/Misc/TAnaManager.cxx) as needed and recompile. No updates to the Makefile should be needed.


[`TRootanaEventLoop`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html
[`Initialize()`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html#a3d6502097ba23177d477dd0b4528aca9
[`BeginRun()`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html#a7adb0f62ab8aff8b9a9f31ce5fea6f0d
[`EndRun()`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html#ab75d515101b4f51fbc0e1a9388b3a09b
[`Finalize()`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html#acc1296767b1faaefde3e5d98fbb35d96
[`ProcessMidasEvent(TDataContainer& dataContainer)`]: https://ladd00.triumf.ca/~daqweb/doc/rootana/html/class_t_rootana_event_loop.html#a41650949d3a59bca09b468af845271d9
[`midas2root`]: src/midas2root.cxx