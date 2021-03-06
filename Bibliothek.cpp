#include "Bibliothek.h"

Bibliothek::Bibliothek( void ) {
    this->errFileNotFound = false;
    this->errFileRead = false;
}

Bibliothek::~Bibliothek( void ) {
}

// Pfad auslesen
string Bibliothek::getPfad() {
	return datei;
}

// Bib-Elemente auslesen
GatterTyp* Bibliothek::getBibElement( string typ ) {
    for( int i = 0; i < bibElemente.size() ; i++ ) {
		if( bibElemente[i]->getName().find(typ) != -1 ) {
			return this->bibElemente[i];
		}
	}
	return NULL;
}

// Bib-Datei ausgeben
void Bibliothek::dateiAusgabe() {
    string line;
    int i;
    ifstream infile( this->datei );
    if( infile.is_open()) {
        i = 1;
        while( getline ( infile, line )) {
            cout << i++ << " " << line <<endl;
        }
	}
}

// Bib-Datei auswerten
void Bibliothek::dateiAuswerten() {
    for( int i = 0; i < bibElemente.size() ; i++ ) {
	  delete bibElemente[i];
	}
	bibElemente.clear(); //Vector zu Beginn der FKT leeren
	ifstream file( datei );
	string line;
    if( file.is_open() ) {
		while( getline ( file, line )) {
			if( line.find( "ARCHITECTURE" ) != -1 ) {
				this->errFileRead = true;
				break;
			}
			if( line.find( "[[Bausteine]]" ) != -1 ) {
				getline ( file, line );
				//Gatterelemente der Liste hinzufuegen
				while( !line.empty() ) {
					if( line.find( "dff" ) != -1 ) {
						bibElemente.push_back( new FlipFlop() );
					} else {
						bibElemente.push_back( new GatterTyp() );
					}
					bibElemente.back()->setName( line ); //Name der einzelnen Bauelementr im Vector speichern
					getline ( file, line );
				}
				//Entitaeten auslesen bis Dateiende
				while( line.find( "#endfile" ) == -1 ) {
					getline ( file, line );
					line = line.substr( 1 );
					string name = line.substr( 0, line.length() - 1 );
					if( name.find( "dff" ) == -1 ) { //Eigenschaften der nicht D-Flip-Flops auslesen und speichern
						for( int i = 0; i < 4; i++ ) {
							getline ( file, line );
							if( line.find( "tpd0" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								getBibElement(name)->setGrundLaufzeit( atof( line.c_str() ));
							}
							if( line.find( "kl" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								getBibElement(name)->setLastFaktor( atof( line.c_str() ));
							}
							if( line.find( "ei" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								getBibElement(name)->setEingaenge( atof( line.c_str() ));
							}
							if( line.find( "cl" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								getBibElement(name)->setLastKapazitaet( atof( line.c_str() ));
							}
						}
					} else { //Eigenschaften des D-Flip-Flops auslesen und speichern
						for( int i = 0; i < 8; i++ ) {
							getline ( file, line );
							if( line.find( "ed" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setEingaenge( getBibElement(name)->getEingaenge() + atof( line.c_str() ));
							}
							if( line.find( "tsetup" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setSetupTime( atof( line.c_str() ));
							}
							if( line.find( "thold" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setHoldTime( atof( line.c_str() ));
							}
							if( line.find( "cd" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setLastKapazitaet( atof( line.c_str() ));
							}
							if( line.find( "et" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setEingaenge( getBibElement(name)->getEingaenge() + atof( line.c_str() ));
							}
							if( line.find( "tpdt" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setGrundLaufzeit( atof( line.c_str() ));
							}
							if( line.find( "kl" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setLastFaktor( atof( line.c_str() ));
							}
							if( line.find( "ct" ) != -1 ) {
								line = line.substr( line.find( ":" ) + 1 );
								((FlipFlop*) getBibElement(name))->setLastKapazitaetClock( atof( line.c_str() ));
							}
						}
					}
					getline ( file, line );
				}
			}

			if( file.fail() ) {
				readError();
			}

			this->errFileRead = false;
        }
    }
    file.close();
}

// Dateipfad einlesen
bool Bibliothek::pfadEinlesen( string pfad ) {
    ifstream file( pfad );
    if( file.is_open()) {
		datei = pfad;
		dateiAuswerten();
		if( this->errFileRead == true ) {
			datei = "";
			return false;
		}
		return true;
	} else {
		return false;
    }
    file.close(); 
}

// Fehler beim Oeffnen
void Bibliothek::openError() {
	if( errFileNotFound ) {
		cout << endl  << "Beim Oeffnen der Datei ist ein Fehler aufgetreten" << endl;
		system( "pause" );
	}
}

// Fehler beim Lesen
void Bibliothek::readError() {
	if( errFileRead ) {
		cout << endl  << "Beim Lesen der Datei ist ein Fehler aufgetreten" << endl;
		system( "pause" );
	}
}