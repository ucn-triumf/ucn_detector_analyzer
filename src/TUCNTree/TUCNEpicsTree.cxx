#include "TUCNEpicsTree.hxx"

TUCNEpicsTree::TUCNEpicsTree(MVOdb* odb, char const* bankname, char const* treename,
                            char const* odbpath){
    /**
     * @brief Base constructor for objects which create trees from MIDAS banks.
     *
     * @param odb Pointer to ODB object to read list of parameter names
     * @param bankname  Name of the MIDAS bank which holds the data, get the
     *                  value from the frontend code
     * @param treename  Name of the TTree to write out to file
     * @param odbpath   Path to the list of parameter names.
     *                  Typically /Equipment/something/Settings/Names
     */

    // save input
    bank = bankname;

    // create tree and timestamp branch
    datatree = new TTree(treename, (std::string(treename)+" TTree").c_str());
    datatree->Branch("timestamp", &timestamp, "timestamp/I" );

    // read names
    std::vector<std::string> names = std::vector<std::string>();
    odb->RSA(odbpath, &names);
    values.resize(names.size());    // prevents segfault on tree fill

    // setup branches
    for (long unsigned int i=0; i<names.size(); i++){

        // don't use default branches
        if (names[i].find("Default") == std::string::npos){

            // can't have colons in the name or leaf list is expanded
            std::replace(names[i].begin(), names[i].end(), ':', '_');

            // replace . with _ for ease of processing later
            std::replace(names[i].begin(), names[i].end(), '.', '_');

            datatree->Branch(names[i].c_str(), &values[i], (names[i]+"/D").c_str());
        }
    }
};

void TUCNEpicsTree::FillTree(TDataContainer& dataContainer){
    /**
     * @brief Fill the trees with data from MIDAS events.
     */

    // Use the sequence bank to see when a new run starts:
    TGenericData *data = dataContainer.GetEventData<TGenericData>(bank);

    if(!data) return;

    // Save the unix timestamp
    timestamp = dataContainer.GetMidasData().GetTimeStamp();

    // Save variables
    for (long unsigned int i=0; i<values.size(); i++){
        values[i] = data->GetFloat()[i];
    }
    datatree->Fill();
};
