void plotXS()
{
	const int nIncidents = 5000000;
    const float thickness = 0.1; //cm
    const float Nv = 4.6e22; // 1/cm3

	vector<float> inEnergyVec;
	vector<float> xsVec;

	int counter = 0;

	for(int iFile = 1900; iFile <= 2400; iFile += 20)
	{
		string fileName("run");
		fileName = fileName + to_string(iFile) + ".root";
		TFile* f = new TFile(fileName.c_str());
        f->Get("6");
        TH1D* h = (TH1D*)f->Get("6");
        int nNeutron = h->GetEntries();

		float en = iFile/1000.; // in MeV
		float xs = 1.0 * nNeutron/nIncidents / Nv / thickness; // cm2
		xs = xs / 1.e-24; // barn

	    cout << "Ei: " << en << ", nNeutron: " <<  nNeutron << ", xs: " << xs << endl;

		inEnergyVec.push_back( en );
		xsVec.push_back( xs );

		++counter;
	}

	// vector2array
	float* inEnergyArray = &inEnergyVec[0];
	float* xsArray = &xsVec[0];

	TGraph *gr = new TGraph(counter, inEnergyArray, xsArray);

    gr->SetLineColor(2);
    gr->SetLineWidth(4);
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(26);
    gr->SetTitle("^{7}Li(p, n)^{7}Be cross section");
    gr->GetXaxis()->SetTitle("proton energy (MeV)");
    gr->GetYaxis()->SetTitle("cross section (barn)");
    gr->Draw("ACP");
}
