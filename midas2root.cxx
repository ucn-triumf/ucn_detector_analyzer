// UCN midas2root converter program

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"
#include "mvodb.h"

class Analyzer: public TRootanaEventLoop{

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
        anaManager = 0; //new TAnaManager(IsOffline(),true);
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

        // iterate through the Edit on start keys and fetch all values
        MVOdb *odb = GetODB();
        TTree *headerTree = new TTree("headerTree", "headerTree");

        // try to get all edit on start values
        std::string path = std::string("/Experiment/Edit on start/");

        std::vector<std::string> varname = std::vector<std::string>();
        std::vector<int> tid = std::vector<int>();
        std::vector<int> num_values = std::vector<int>();
        std::vector<int> total_size = std::vector<int>();
        std::vector<int> item_size = std::vector<int>();
        MVOdbError* error = new MVOdbError();

        MVOdb *odb_editonstart = odb->Chdir(path.c_str());
        odb_editonstart->ReadDir(&varname, &tid, &num_values, &total_size, &item_size, error);

        // make branches and get odb info
        std::vector<std::string> odb_header = std::vector<std::string>();
        odb_header.resize(varname.size()); // prevents segfault on tree fill

        for (long unsigned int i=0; i<varname.size(); i++){
            odb->RS((path+varname[i]).c_str(), &odb_header[i]);
            headerTree->Branch(varname[i].c_str(), &odb_header[i]);
        }
        headerTree->Fill();

        // print header info
        for (long unsigned int i=0; i<varname.size(); i++){
            std::cout << varname[i] << ": " << odb_header[i] << std::endl;
        }
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

