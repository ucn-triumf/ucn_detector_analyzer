// UCN midas2root converter program

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"
#include "mvodb.h"

class Analyzer: public TRootanaEventLoop{

private:
    std::vector<const char*> header_names = {"/Experiment/Edit on start/Run Title",
                                             "/Experiment/Edit on start/Experiment number",
                                             "/Experiment/Edit on start/Shifters",
                                             "/Experiment/Edit on start/Comment",
                                             "/Runinfo/Start time",
                                             "/Runinfo/Stop time",
                                             };
    int run_number;

public:

    // Two analysis managers.  Define and fill histograms in analysis manager.
    TAnaManager *anaManager;
    TUCNAnaViewer3 *anaViewer;

    Analyzer(){
        SetOutputFilename("ucn_run_");
        DisableAutoMainWindow();
        UseBatchMode();
        SetOnlineName("midas2root");
        anaManager = 0;
        anaViewer = 0;
    };

    virtual ~Analyzer() {};

    void Initialize(){

        #ifdef HAVE_THTTP_SERVER
            std::cout << "Using THttpServer in read/write mode" << std::endl;
            SetTHttpServerReadWrite();
        #endif

        // Start the anaManager with flag to write trees.
        anaManager = 0; //new TAnaManager(IsOffline(), GetODB(), true);
        anaViewer  = 0; //new TUCNAnaViewer3();
    }

    void InitManager(){

        if(anaManager)
            delete anaManager;
        anaManager = new TAnaManager(IsOffline(), GetODB(), true);
        if(anaViewer)
            delete anaViewer;
        anaViewer  = new TUCNAnaViewer3();
    }

    void BeginRun(int transition, int run, int time){

        InitManager();
        anaManager->BeginRun(transition, run, time);

        /// Make header

        // iterate through the Edit on start keys and fetch all values
        MVOdb *odb = GetODB();
        TTree *headerTree = new TTree("header", "header TTree");

        // get header string values
        std::vector<std::string> header_val = std::vector<std::string>();
        header_val.resize(header_names.size()); // prevents segfault on tree fill
        std::string name;   // name of header without full path

        for (long unsigned int i=0; i<header_names.size(); i++){

            // get name without path
            name = std::string(header_names[i]);
            name = name.substr(name.find_last_of('/')+1);

            // read string and make branch
            odb->RS(header_names[i], &header_val[i]);
            std::cout << name << ": \t" << header_val[i] << std::endl;
            headerTree->Branch(name.c_str(), &(header_val[i]));
        }

        // get run number
        odb->RI("/Runinfo/Run number", &run_number); // read string
        std::cout << "Run number: \t" << run_number << std::endl;
        headerTree->Branch("Run number", &run_number);

        // fill tree
        headerTree->Fill();
    }

    bool ProcessMidasEvent(TDataContainer& dataContainer){
        if(!anaManager)
            InitManager();

        float PSDMax, PSDMin;
        anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);
        anaManager->ProcessMidasEvent(dataContainer);

        return true;
    }
};

int main(int argc, char *argv[]){
    Analyzer::CreateSingleton<Analyzer>();
    return Analyzer::Get().ExecuteLoop(argc, argv);
}

