//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "Run.hh"
#include "EventAction.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4StepStatus.hh"
#include "G4ParticleTypes.hh"
#include "G4IonTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4Material.hh"
#include "G4HadronicProcessStore.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(EventAction* event)
:G4UserTrackingAction(), fEventAction(event)
{
   fTimeBirth = fTimeEnd = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
  Run* run = static_cast<Run*>(
       G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  G4ParticleDefinition* particle = track->GetDefinition();
  G4String name     = particle->GetParticleName();
  G4double meanLife = particle->GetPDGLifeTime();
  G4double ekin     = track->GetKineticEnergy();
  fTimeBirth       = track->GetGlobalTime();

  //count secondary particles
  if (track->GetTrackID() > 1)  run->ParticleCount(name,ekin,meanLife);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
  //G4cout << G4endl;

  Run* run = static_cast<Run*>(
       G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();
  
  const G4ParticleDefinition* particle = track->GetParticleDefinition();
  //const G4ProcessVector* plist = particle->GetProcessManager()->GetProcessList();

  G4String name     = particle->GetParticleName();

  //const G4DynamicParticle* part = track->GetDynamicParticle();

#if 0
  ////////
  G4String elname, symbol;
  G4double a, z;
  G4int nel;
  G4double density = 0.534*g/cm3;
  a = 6.94*g/mole;

  G4Element* elLi  = new G4Element(elname="Lithium", symbol="Li" , z= 3., a);
  G4Material* Lithium = new G4Material(elname="Lithium",density, nel=1);
  Lithium->AddElement(elLi,1);
#endif

#if 0
  for(size_t j=0; j<plist->size(); j++)
  {
	  G4String procName = (*plist)[j]->GetProcessName();

	  G4HadronicProcessStore* store = G4HadronicProcessStore::Instance();

	  G4double sigma = 0.;
	  //sigma = store->GetCrossSectionPerAtom(particle, 2.2 * MeV, (*plist)[j], elLi, Lithium);

	  G4HadronicProcess* pro = store->FindProcess(particle, fHadronInelastic);

	  G4CrossSectionDataStore* ds = pro->GetCrossSectionDataStore();
	  ds->DumpPhysicsTable(*particle);
	  //ds->PrintCrossSectionHtml();

	  G4cout << " ------ process: " << pro << G4endl;
	  //sigma = pro->GetElementCrossSection(part, elLi, Lithium);

	  G4cout << "particle: " << name << ", process " << procName << ", xs: " << sigma << G4endl;
  }
#endif
  //delete Lithium;


  G4double meanLife = particle->GetPDGLifeTime();
  G4double ekin     = track->GetKineticEnergy();
  fTimeEnd         = track->GetGlobalTime();
  if ((particle->GetPDGStable())&&(ekin == 0.)) fTimeEnd = DBL_MAX;

#if 0
  //if(ekin > 0.01 * MeV) 
  {
	  if(track->GetParentID()==1) return;
      G4cout << "track ID: " << track->GetTrackID() 
		  << ", ekin: " << ekin << ", particle: " << particle->GetParticleName() << G4endl;
      G4cout << "parent track ID: " << track->GetParentID() << G4endl;
  }
#endif
  
  // count population of ions with meanLife > 0.
  if ((G4IonTable::IsIon(particle))&&(meanLife != 0.)) {
    G4int id = run->GetIonId(name);
    G4double unit = analysis->GetH1Unit(id);
    G4double tmin = analysis->GetH1Xmin(id)*unit;
    G4double tmax = analysis->GetH1Xmax(id)*unit;
    G4double binWidth = analysis->GetH1Width(id)*unit;

    G4double t1 = std::max(fTimeBirth,tmin);
    G4double t2 = std::min(fTimeEnd  ,tmax);
    for (G4double time = t1; time<t2; time+= binWidth)
       analysis->FillH1(id,time);
  }

 // keep only emerging particles
 G4StepStatus status = track->GetStep()->GetPostStepPoint()->GetStepStatus();
 if (status != fWorldBoundary) 
 {
	 //G4cout << "not a emerging particle" << G4endl;
	 return; 
 }

 fEventAction->AddEflow(ekin);
 run->ParticleFlux(name,ekin);


 // histograms: energy flow and activities of emerging particles
 
 G4int ih1 = 0, ih2 = 0; 
 G4String type   = particle->GetParticleType();      
 G4double charge = particle->GetPDGCharge();
 G4double time   = track->GetGlobalTime();
 if (charge > 3.)  {ih1 = 10; ih2 = 20;}
 else if (particle == G4Gamma::Gamma())       {ih1 = 4;  ih2 = 14;}
 else if (particle == G4Electron::Electron()) {ih1 = 5;  ih2 = 15;}
 else if (particle == G4Positron::Positron()) {ih1 = 5;  ih2 = 15;}
 else if (particle == G4Neutron::Neutron())   {ih1 = 6;  ih2 = 16;}
 else if (particle == G4Proton::Proton())     {ih1 = 7;  ih2 = 17;}
 else if (particle == G4Deuteron::Deuteron()) {ih1 = 8;  ih2 = 18;}
 else if (particle == G4Alpha::Alpha())       {ih1 = 9;  ih2 = 19;}
 else if (type == "nucleus")                  {ih1 = 10; ih2 = 20;}
 else if (type == "baryon")                   {ih1 = 11; ih2 = 21;}
 else if (type == "meson")                    {ih1 = 12; ih2 = 22;}
 else if (type == "lepton")                   {ih1 = 13; ih2 = 23;};
 if (ih1 > 0) analysis->FillH1(ih1,ekin);
 if (ih2 > 0) analysis->FillH1(ih2,time);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
