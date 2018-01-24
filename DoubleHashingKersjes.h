#ifndef DOUBLEHASHINGKERSJES_H
#define DOUBLEHASHING_H

// DoubleHashingKersjes.h
//
// UE Algorithmen und Datenstrukturen - Universitaet Wien
// Container - Projekt
// Doppelstreuwertverfahren (DoubleHashing)
// Quicksort
//
// Christian Kersjes - a1147684
//

/*
 * if(this.programm instanceof SkyNet || this.programm.reader instanceof PRISM)
 * {
 * 		programm.selfdestroy();
 * }else{
 * 		programm.main();
 * }
 *
 */

#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "Container.h"

class DatenstrukturLeerException;

class DatenstrukturLeerException : public ContainerException {
public:
    virtual const char * what() const throw() { return "DoubleHashingKersjes: Datenstruktur ist leer!"; }
};

template <typename E>
class DoubleHashingKersjes : public Container<E> {
    
    mutable E * werteFeld;
    mutable E * sortWerte;
    
    //mutable size_t * primZahlen;
    
    enum Status {frei, belegt, wiederFrei};
    enum Sortierstatus {sortiert, sortieren};
    
    Status * statusFeld;
    mutable Sortierstatus sortStatus;
    
    mutable size_t anzahlMax;
    mutable size_t anzahlAktuell;
    
    virtual void sort(size_t linkeGrenze, size_t rechteGrenze) const;
    
    
public:
    DoubleHashingKersjes<E>( );
    
    ~DoubleHashingKersjes<E>( ) {
        delete [] sortWerte;
        delete [] werteFeld;
        delete [] statusFeld;
        //delete [] primZahlen;
    }
    
    using Container<E>::add;
    virtual void add( const E e[], size_t len );
    
    using Container<E>::remove;
    virtual void remove( const E zuloeschenderWert[], size_t s );
    
    virtual bool member( const E& e ) const;
    virtual size_t size( ) const {return anzahlAktuell; };
    
    virtual size_t apply( const Functor<E>& f, Order order=dontcare ) const;
    
    virtual E min( ) const;
    virtual E max( ) const;
    
    virtual std::ostream& print( std::ostream& o ) const;
    
    virtual size_t primzahl(size_t minGroesse) const;
    
    virtual void hashFunktion(const E& zuHashenderEintrag) const;
    
    virtual void hashFunktionErweiterung(const E& zuHashenderEintrag, Status * statusFeldTemp) const;
    
};

template <typename E>
DoubleHashingKersjes<E>::DoubleHashingKersjes( ) {
    sortWerte = new E[7];
    sortStatus = sortieren;
    anzahlMax = 7;
    anzahlAktuell =0;
    werteFeld = new E[anzahlMax];
    statusFeld = new Status[anzahlMax];
    
    /*primZahlen = new size_t[17];
    
    
    primZahlen[0] = 53;
    primZahlen[1] = 97;
    primZahlen[2] = 193;
    primZahlen[3] = 389;
    primZahlen[4] = 769;
    primZahlen[5] = 1543;
    primZahlen[6] = 3079;
    primZahlen[7] = 6151;
    primZahlen[8] = 12289;
    primZahlen[9] = 24593;
    primZahlen[10] = 49157;
    primZahlen[11] = 98317;
    primZahlen[12] = 196613;
    primZahlen[13] = 393241;
    primZahlen[14] = 786433;
    primZahlen[15] = 1572869;
    primZahlen[16] = 3145739;*/
 
    
    for (size_t h=0; h<anzahlMax; h++) {
        statusFeld[h] = frei;
    }
}

// Kleinster Wert
template <typename E>
E DoubleHashingKersjes<E>::min() const{
    
    // Wenn die Datenstruktur leer ist, eine Fehlermeldung werfen
    if (anzahlAktuell == 0) {
        throw DatenstrukturLeerException();
    }
    
    if(anzahlAktuell==1){
        for (size_t h=0; h<anzahlMax; h++) {
            if (statusFeld[h] == belegt) {
                return werteFeld[h];
            }
        }
    }
    
    E min;

    if (sortStatus==sortieren) {
        // LINEARE SUCHE
        //E min;
        bool flag = false;
        
        min = werteFeld[0];
        
        for (size_t o=0; o<anzahlMax; o++) {
            if (statusFeld[o] == belegt) {
                if (flag==false) {
                    min = werteFeld[o];
                    flag = true;
                }
                
                if (min > werteFeld[o]) {
                    min = werteFeld[o];
                }
            }
        }

    }else{
        min = sortWerte[0];
    }
    
    return min;
}

// Groesster Wert
template <typename E>
E DoubleHashingKersjes<E>::max() const{
    
    // Wenn die Datenstruktur leer ist, eine Fehlermeldung werfen
    if (anzahlAktuell == 0) {
        throw DatenstrukturLeerException();
    }
    
    if(anzahlAktuell==1){
        for (size_t h=0; h<anzahlMax; h++) {
            if (statusFeld[h] == belegt) {
                return werteFeld[h];
            }
        }
    }
    
    E max;
    if (sortStatus == sortieren) {
        // Lineare Suche
        max = werteFeld[0];
        bool flag = false;
        
        // Array zum Sortieren befuellen
        for (size_t o=0; o<anzahlMax; o++) {
            if (statusFeld[o] == belegt) {
                if (flag==false) {
                    max = werteFeld[o];
                    flag = true;
                }
                
                if (werteFeld[o] > max) {
                    max = werteFeld[o];
                }
            }
        }
    }else{
        max = sortWerte[anzahlAktuell-1];
    }

    return max;
}

// Werte sortieren
template <typename E>
size_t DoubleHashingKersjes<E>::apply(const Functor<E>& f, Order order) const{
    size_t anzEleFunctor = 0;
    
    // Wenn es nichts zu sortieren gibt
    if (anzahlAktuell == 0) {
        return anzEleFunctor;
    }
    
    // dontcare
    //    beliebige Reihenfolge (kann auch sortiert sein, soll aber die Werte schnellstmöglich liefern,
    if (order == dontcare) {
        
        // wenn es kein sortWerte gibt
        if (sortStatus == sortieren) {
            // delete [] sortWerte;
            // vorhandene Eintraege durchlaufen
            for (size_t h=0; h<anzahlMax; h++) {
                if (statusFeld[h] == belegt) {
                    anzEleFunctor++;
                    // solange der Functor true liefert weiter laufen
                    // wenn false, dann die Anzahl der durch den Functor durchlaufenden Elemente liefern
                    if (!f(werteFeld[h])) {
                        break;
                    }
                }
            }
        }else{
            // vorhandene Eintraege durchlaufen
            for (size_t h=0; h<anzahlAktuell; h++) {
                anzEleFunctor++;
                
                // solange der Functor true liefert weiter laufen
                // wenn false, dann die Anzahl der durch den Functor durchlaufenden Elemente liefern
                if (!f(sortWerte[h])) {
                    break;
                }
            }
        }
        
    }else{
    
    // Wenn sich am werteFeld etwas getan hat, oder noch nie sortiert wurde
    if (sortStatus==sortieren) {
        
        delete [] sortWerte;
        sortWerte = new E[anzahlAktuell];
        
        // std::cout << reinterpret_cast<void*>(sortWerte) << std::endl;
        
        size_t counter =0;
        
        // Array zum Sortieren befuellen
        for (size_t h=0; h<anzahlMax; h++) {
            if (statusFeld[h] == belegt) {
                sortWerte[counter] = werteFeld[h];
                counter++;
            }
        }
        
        size_t linkeGrenze = 0;
        size_t rechteGrenze = anzahlAktuell-1;
        
        if(anzahlAktuell>1){
            sort(linkeGrenze, rechteGrenze);
        }
        sortStatus = sortiert;
    }
    
    // ascending
    //    bedeutet aufsteigende Sortierreihenfolge (der kleinste in der Datenstruktur gespeicherte Wert zuerst),
    if (order == ascending) {
        
        // vorhandene Eintraege durchlaufen
        for (size_t h=0; h<anzahlAktuell; h++) {
            anzEleFunctor++;
            
            // solange der Functor true liefert weiter laufen
            // wenn false, dann die Anzahl der durch den Functor durchlaufenden Elemente liefern
            if (!f(sortWerte[h])) {
                break;
            }
        }
    }
    
    // descending
    //    absteigende Sortierreihenfolge
    if (order == descending) {
        
        // vorhandene Eintraege durchlaufen
        for (long h=anzahlAktuell-1; h>=0; h--) {
            anzEleFunctor++;
            
            // solange der Functor true liefert weiter laufen
            // wenn false, dann die Anzahl der durch den Functor durchlaufenden Elemente liefern
            if (!f(sortWerte[h])) {
                break;
            }
        }
    }
}
    return anzEleFunctor;
}

// Quicksort
template <typename E>
void DoubleHashingKersjes<E>::sort(size_t linkeGrenze, size_t rechteGrenze) const {
    
    size_t l = linkeGrenze;
    size_t r = rechteGrenze;
    
    E wertTemp;
    
    size_t mitte;
    
    if((linkeGrenze+rechteGrenze)>=2){
        mitte = (linkeGrenze + rechteGrenze) / 2;
    }else{
        mitte = r;
    }
    
    // Mit dem mittleren Wert anfangen zu Laufzeitverbesserung
    E pivotElement = sortWerte[mitte];
    //E pivotElement = sortWerte[r];
    
    // Nun die Werte mit einem Pointer von link (l) und rechts (r) durchlaufen
    // um ein Element zu finden, welches für l groesser oder gleich dem pivotElement ist
    // und für r kleiner oder gleich dem pivotElement
    while (l <= r) {
        while (pivotElement > sortWerte[l]) {
            l++;
        }

        while (sortWerte[r] > pivotElement) {
            r--;
        }
        
        // zur Kontrolle, das die beiden Pointer sich nicht gekreuzt haben
        if (l <= r) {
            // aufteilen in 2 Teillisten
            // Element < Pivotelement in linke Teilliste
            // Element > Pivotelement in rechte Teilliste
            // Element == Pivotelement bleibt wo es ist. Sind somit in beiden Listen vorhanden
            // Am Ende Elemente linke Liste <= Elemente rechte Liste
            wertTemp = sortWerte[l];
            sortWerte[l] = sortWerte[r];
            sortWerte[r] = wertTemp;
            l++;
            r--;
        }
    }
    
    // nun die Methode rekursiv aufrufen, so das alle Werte sortiert werden
    // Genauer gesagt, werden die Teillisten in sich wieder in 2 Teillisten aufgeteilt und sortiert.
    // Solange bis der gesamte Inhalt sortiert ist. Also solange r > linkeGrenze und l < rechteGrenze
    // wenn beides nicht gegeben, ist alles sortiert.
    if (linkeGrenze < r) {
        sort(linkeGrenze, r);
    }
    
    if (l < rechteGrenze) {
        sort(l, rechteGrenze);
    } 
}

// Werte entfernen
template <typename E>
void DoubleHashingKersjes<E>::remove(const E zuloeschenderWert[], size_t s){
    
    for(size_t k=0; k<s ; k++) {

    bool flag = false;
    
    if(!((anzahlAktuell==0) && (flag == true))) {
        
        //if (member(zuloeschenderWert[k])) {
            size_t tempHash = hashValue(zuloeschenderWert[k]);
            
            size_t tempSchluessel;
            
            size_t j = 0;
        
        
        
            // erstellen des ersten moeglichen schluesselWert
            tempSchluessel = (tempHash % anzahlMax);
            
            // pruefen ob an der Position der zu suchende Eintrag vorhanden
            while (!((statusFeld[tempSchluessel] == belegt) && (werteFeld[tempSchluessel] == zuloeschenderWert[k]) ) ) {
                j++;
                
                // neuen  schluesselWert erstellen, da beim vorherigen nicht der gesuchte Eintrag
                tempSchluessel = ((tempHash % anzahlMax) + (j * (1+(tempHash % (anzahlMax-2))))) % anzahlMax;
                
                if(tempSchluessel == (tempHash % anzahlMax)){
                    flag = true;
                    break;
                }
                
                if (statusFeld[tempSchluessel] == frei) {
                    flag = true;
                    break;
                }
                
            }
            
            if(flag == false){
            // gesuchter Eintrag wird gelöscht
            statusFeld[tempSchluessel] = wiederFrei;
            
            anzahlAktuell--;
            
            sortStatus = sortieren;
            }
        //}
    }
    }
}

// Wert an der richtigen Position abspeichern
template <typename E>
void DoubleHashingKersjes<E>::hashFunktion(const E& zuHashenderEintrag) const {
    size_t tempHash = hashValue(zuHashenderEintrag);
    size_t tempSchluessel;
    size_t j = 0;
    tempSchluessel = (tempHash % anzahlMax);
    
    while (statusFeld[tempSchluessel] == belegt ) {
        j++;
        
        //k mod 7 + i * (1+(k mod 5)) mod 7
        tempSchluessel = ((tempHash % anzahlMax) + (j * (1+(tempHash % (anzahlMax-2))))) % anzahlMax;
    }
    
    
    werteFeld[tempSchluessel] = zuHashenderEintrag;
    
    anzahlAktuell++;
    
    statusFeld[tempSchluessel] = belegt;
    
    sortStatus = sortieren;
}

template <typename E>
void DoubleHashingKersjes<E>::hashFunktionErweiterung(const E& zuHashenderEintrag, Status * statusFeldTemp) const {
    size_t tempHash = hashValue(zuHashenderEintrag);
    size_t tempSchluessel;
    size_t j = 0;
    
    tempSchluessel = (tempHash % anzahlMax);
    
    while (statusFeldTemp[tempSchluessel] == belegt || statusFeldTemp[tempSchluessel] == wiederFrei) {
        j++;
        tempSchluessel = ((tempHash % anzahlMax) + (j * (1+(tempHash % (anzahlMax-2))))) % anzahlMax;
    }
    
    werteFeld[tempSchluessel] = zuHashenderEintrag;
    
    anzahlAktuell++;
    
    statusFeldTemp[tempSchluessel] = belegt;
    
    sortStatus = sortieren;
}

template <typename E>
size_t DoubleHashingKersjes<E>::primzahl(size_t minGroesse) const {

   /* for (size_t i =0; i <17; i++) {
        if (primZahlen[i] > minGroesse) {
            return primZahlen[i];
        }
    }
    return primZahlen[16];*/
    
     size_t teiler;
    
    /*
     * Zunächst, was ist eine Primzahl. Dazu habe ich 2 Definitionen gefunden
     * - Eine Primzahl ist eine natürliche Zahl, die größer als eins und nur durch sich selbst und durch eins
     * teilbar ist.
     * - Eine Primzahl ist eine natürliche Zahl mit genau zwei natürlichen Zahlen als Teiler
     */
    
    // Prüfen ob die eingegebene Zahl 1 ist. 1 ist keine Primzahl (siehe die 2 Definitionen oben)
    if(minGroesse == 1)
    {
        // wenn 1, Zahl vergroessern
        minGroesse++;
    }
    
    // Prüfen ob die eingegebene Zahl 2 ist. 2 ist die einzige gerade Primzahl
    if(minGroesse == 2)
    {
        return 7;
    }
    
    // Prüfen ob die eingegebene Zahl 3 ist. 3 ist eine Primzahl
    // da ich aber nach der Überprüfung ob gerade/ungerade den teiler auf 3 setze und ich
    // eine do while schleife habe, würde die 3 nicht als Primzahl berücksichtigs werden,
    // da er auf jedenfall in die erste Abbruchbedingung gelangt.
    if(minGroesse <= 3)
    {
        return 7;
    }
    
    // Prüfen ob die Zahl gerade oder ungerade ist (solange kein Rest vorhanden),
    // da Primzahlen nur ungerade sind
    teiler = 2;
    
    while(!(minGroesse%teiler))
    {
        minGroesse++;
    }
    
    // Wenn es sich um eine ungerade Zahl handelt, bereite ich den Teiler für
    // den ungeraden Bereich vor.
    teiler=3;
    
    // Schleifendurchlauf für Versuche solange der Teiler <= der Wurzel der eingegebenen Zahl.
    // Dies gibt einiges an Performance, da ich weniger prüfen muss. Dies rührt daher, das der grösste
    // echte Teiler einer Zahl nicht grösser als dessen Wurzel sein kann.
    do
    {
        // Wenn die Moduloüberprüfung keinen Rest ergibt, ist die Zahl durch den Teiler teilbar
        // dies bedeutet, das die Definition einer Primzahl nicht gegeben ist (nur durch 1 und sich selbst teilbar)
        if (!(minGroesse%teiler))
        {
            // naechst hoehere Zahl ausprobieren um sich so an die Primzahl>minGroesse
            // heran zu tasten
            minGroesse++;
            minGroesse++;
            
            // Teiler reseten. Zunächst auf 1 um wieder mit dem Ablauf kompatibel
            // zu sein, da im naechsten Step +2 gesetzt wird und somit der Teiler
            // wieder 3 ist.
            teiler = 1;
        }
        // erhöhung des Teilers um 2 und damit auslassen der geraden Zahlen
        teiler = teiler + 2;
    }
    while (teiler <= sqrt(minGroesse));
    
    // Die Zahl ist durch keine Zahl zwischen 2 bis zur Wurzel der eingegebenen Zahl teilbar
    // und somit eine Primzahl
    
    return minGroesse;
}

template <typename E>
void DoubleHashingKersjes<E>::add( const E neuerEintrag[], size_t anzahlNeue ) {
    
    if (!(anzahlNeue == 0)) {
        
        // anzahlMaxMoeglicheEintraege - ca. 70%, damit das Array nie mehr als 70% gefüllt ist
        // für eine bessere Performance
        size_t optimaleAnzahlMax = floor(anzahlMax*0.7);
        
        // ueberprüfen ob werteFeld erweitert werden muss
        if ((anzahlAktuell + anzahlNeue) >= optimaleAnzahlMax) {
            
            Status * statusFeldTemp;
            E * werteFeldTemp;
            
            werteFeldTemp= new E[anzahlAktuell];
            size_t anzahlMaxTemp = anzahlMax;
            
            size_t counter=0;
            
            // uebertragen der vorhandenen Eintraege in das Temp. Array zum
            // zwischenspeichern
            for (size_t i=0; i<anzahlMaxTemp; i++) {
                if (statusFeld[i] == belegt) {
                    werteFeldTemp[counter] = werteFeld[i];
                    counter++;
                }
            }
            
            size_t minGroesse = (anzahlAktuell + anzahlNeue)*2;
            
            anzahlMax = primzahl(minGroesse);
            
            // loeschen des Arrays und anschliessend neu initialisieren, mit der
            // erweiterten Groesse
            delete [] werteFeld;
            werteFeld = new E[anzahlMax];
            
            sortStatus = sortieren;
            
            statusFeldTemp = new Status[anzahlMax];
            
            for (size_t h=0; h<anzahlMax; h++) {
                statusFeldTemp[h] = frei;
            }
            
            anzahlAktuell=0;
            
            for (size_t z=0; z<counter ; z++) {
                hashFunktionErweiterung(werteFeldTemp[z], statusFeldTemp);
            }
            
            delete [] werteFeldTemp;
            
            delete [] statusFeld;
            statusFeld = new Status[anzahlMax];
            
            for (size_t g=0; g<anzahlMax; g++) {
                statusFeld[g] = statusFeldTemp[g];
            }
            delete [] statusFeldTemp;
        }
        
        // alle neuen Eintraege durchlaufen
        for (size_t a=0; a<anzahlNeue; a++) {
            // prüfen ob der Wert schon gespeichert ist
            // wenn nicht dann eintragen, sonst zum naechsten Eintrag
            if (!member(neuerEintrag[a])) {
                
                hashFunktion(neuerEintrag[a]);
                sortStatus = sortieren;
            }//if
        }//for
    }//if
}//add

template <typename E>
bool DoubleHashingKersjes<E>::member( const E& zuSuchenderEintrag ) const {
    
    
    if (anzahlAktuell==0) {
        return false;
    }
    
    
    size_t tempHash = hashValue(zuSuchenderEintrag);
    
    size_t tempSchluessel;
    
    size_t j = 0;
    
    // erstellen des ersten moeglichen schluesselWert
    tempSchluessel = (tempHash % anzahlMax);
    
    // pruefen ob an der Position der zu suchende Eintrag vorhanden
    while (true) {
        
        if ((statusFeld[tempSchluessel] == belegt)){
            if (werteFeld[tempSchluessel] == zuSuchenderEintrag){
                return true;
            }else{
                j++;
                // neuen  schluesselWert erstellen, da beim vorherigen nicht der gesuchte Eintrag
                tempSchluessel = ((tempHash % anzahlMax) + (j * (1+(tempHash % (anzahlMax-2))))) % anzahlMax;
            }
        }
        
        if ((statusFeld[tempSchluessel] == wiederFrei)) {
            j++;
            // neuen  schluesselWert erstellen, da beim vorherigen nicht der gesuchte Eintrag
            tempSchluessel = ((tempHash % anzahlMax) + (j * (1+(tempHash % (anzahlMax-2))))) % anzahlMax;
        }
        

        if(tempSchluessel == (tempHash % anzahlMax)){
            return false;
        }
        
        
        // wenn auf ein freies Element verwiesen wird kann der zuSuchendeEintrag
        // nicht abgespeichert sein, da er bis dorthin schon hätte gefunden sein muessen.
        if (statusFeld[tempSchluessel] == frei) {
            return false;
        }
    }
    
    // gesuchter Eintrag nicht vorhanden
    // nur Fallback, eigentlich nie zu erreichen
    return false;
}

// IMPLEMENTIEREN
template <typename E>
std::ostream& DoubleHashingKersjes<E>::print( std::ostream& o ) const {
    o << "DoubleHashingKersjes [ anzahlAktuelleEintraege=" << anzahlAktuell << " anzahlMaxMoeglicheEintraege=" << anzahlMax << " werteFeld=";
    for (size_t i = 0; i < anzahlMax; ++i) (statusFeld[i]==belegt) ? o << ' ' << i << ":" << werteFeld[i] : o << ' ' << i << ": NULL";
    //o << " statusFeld=";
    //for (size_t i = 0; i < anzahlMax; ++i) o << ' ' << i << ":" << statusFeld[i];
    o << " sortWerte=";
    //for (size_t i = 0; i < anzahlAktuell; ++i) o << ' ' << i << ":" << sortWerte[i];
    o << " ]";
    return o;
}

#endif //DOUBLEHASHINGKERSJES_H