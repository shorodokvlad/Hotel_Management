#include "Administrator.h"
#include "Angajat.h"
#include "Data.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>

using namespace std;

int Administrator::nextAngajatId = 1;

void setColorAdmin(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

string Administrator::criptareDecriptare(const string& text, char key)
{
    string output = text;
    for (size_t i = 0; i < text.length(); ++i)
    {
        output[i] = text[i] ^ key;
    }
    return output;
}

void Administrator::verificaSauCreeazaFisierAdmin(const string& defaultUser, const string& defaultPass)
{
    ifstream file("data/admin_creds.txt");
    if (!file.good())
    {
        file.close();
        cout << "\nPrima rulare sau fisierul de credentiale lipseste. Se creeaza credentiale default: \nLogin: ";
        setColorAdmin(14);
        cout << defaultUser << endl;
        setColorAdmin(7);
        cout << "Parola: ";
        setColorAdmin(14);
        cout << defaultPass << endl;

        ofstream outfile("data/admin_creds.txt");
        if (outfile.is_open())
        {
            outfile << defaultUser << endl;
            outfile << criptareDecriptare(defaultPass, XOR_KEY) << endl;
            outfile.close();
        }
        else
        {
            setColorAdmin(12);
            cerr << "Eroare: Nu s-a putut crea fisierul de credentiale admin_creds.txt!" << endl;
        }
    }
    else
    {
        file.close();
    }
}

Administrator::Administrator(Hotel* h, const string& adminFileUser, const string& adminFilePass) : hotel(h)
{
    verificaSauCreeazaFisierAdmin(adminFileUser, adminFilePass);
    ifstream file("data/admin_creds.txt");
    if (file.is_open())
    {
        getline(file, username);
        getline(file, encryptedPassword);
        file.close();
    }
    else
    {
        setColorAdmin(12);
        cerr << "Eroare: Nu s-au putut citi credentialele administratorului!" << endl;
        cerr << "Se folosesc credentialele default compilate." << endl;
        username = adminFileUser;
        encryptedPassword = criptareDecriptare(adminFilePass, XOR_KEY);
    }
    incarcaAngajati();
}

Administrator::~Administrator() {}


bool Administrator::login()
{
    string inputUser, inputPass;
    setColorAdmin(9);
    cout << "\n--- Login Administrator ---" << endl;

    setColorAdmin(8);
    cout << "Username: ";
    setColorAdmin(14);
    cin >> inputUser;

    setColorAdmin(8);
    cout << "Parola: ";
    setColorAdmin(14);
    cin >> inputPass;

    if (inputUser == username && criptareDecriptare(inputPass, XOR_KEY) == encryptedPassword)
    {
        return true;
    }
    else
    {
        setColorAdmin(12);
        cout << "\nUsername sau parola incorecta!" << endl;
        return false;
    }
}

void Administrator::salveazaCredentialeNoi(const string& newUsername, const string& newPassword)
{
    username = newUsername;
    encryptedPassword = criptareDecriptare(newPassword, XOR_KEY);
    ofstream file("data/admin_creds.txt");
    if (file.is_open())
    {
        file << username << endl;
        file << encryptedPassword << endl;
        file.close();
    }
    else
    {
        setColorAdmin(12);
        cerr << "\nEroare: Nu s-au putut salva noile credentiale!" << endl;
    }
}

void Administrator::incarcaAngajati()
{
    ifstream file("data/Angajati.txt");
    if (!file.is_open())
    {
        return;
    }

    string line;
    int maxId = 0;
    while (getline(file, line))
    {
        stringstream ss(line);
        string idStr, nume, prenume, cnp, functie, salariuStr, dataAngStr;

        if (!getline(ss, idStr, ',')) continue;
        if (!getline(ss, nume, ',')) continue;
        if (!getline(ss, prenume, ',')) continue;
        if (!getline(ss, cnp, ',')) continue;
        if (!getline(ss, functie, ',')) continue;
        if (!getline(ss, salariuStr, ',')) continue;
        if (!getline(ss, dataAngStr)) continue;

        try
        {
            int id = stoi(idStr);
            double salariu = stod(salariuStr);

            int zi, luna, an;
            if (sscanf(dataAngStr.c_str(), "%d.%d.%d", &zi, &luna, &an) != 3)
            {
                setColorAdmin(12);
                cerr << "\nEroare la parsarea datei pentru angajat: " << dataAngStr << endl;
                continue;
            }
            Data dataAngajare(zi, luna, an);
            if (!dataAngajare.esteValida())
            {
                setColorAdmin(12);
                cerr << "\nData angajare invalida pentru angajat: " << dataAngStr << endl;
                continue;
            }

            angajati.emplace_back(id, nume, prenume, cnp, functie, salariu, dataAngajare);
            if (id > maxId)
            {
                maxId = id;
            }
        }
        catch (const invalid_argument& ia)
        {
            setColorAdmin(12);
            cerr << "\nArgument invalid la conversie: " << ia.what() << " pe linia: " << line << endl;
        }
        catch (const out_of_range& oor)
        {
            setColorAdmin(12);
            cerr << "\nValoare in afara intervalului la conversie: " << oor.what() << " pe linia: " << line << endl;
        }
    }
    nextAngajatId = maxId + 1;
    file.close();
}

void Administrator::salveazaDateAngajati() const
{
    ofstream file("data/Angajati.txt");
    if (!file.is_open())
    {
        setColorAdmin(12);
        cerr << "\nEroare la deschiderea Angajati.txt pentru salvare!" << endl;
        return;
    }
    for (const auto& angajat : angajati)
    {
        file << angajat.getIdAngajat() << ","
             << angajat.getNume() << ","
             << angajat.getPrenume() << ","
             << angajat.getCNP() << ","
             << angajat.getFunctie() << ","
             << angajat.getSalariu() << ","
             << angajat.getDataAngajare().toString() << endl;
    }
    file.close();
}

void Administrator::adaugaAngajat()
{
    Angajat angajatNou;
    cin >> angajatNou;

    for (const auto& ang : angajati)
    {
        if (ang.getCNP() == angajatNou.getCNP())
        {
            setColorAdmin(12);
            cout << "\nEroare: Un angajat cu acest CNP (" << angajatNou.getCNP() << ") exista deja (ID: " << ang.getIdAngajat() << ")." << endl;
            return;
        }
    }
    if (!angajatNou.getDataAngajare().esteValida())
    {
        setColorAdmin(12);
        cout << "\nDatele introduse pentru angajat nu sunt complete sau valide. Angajatul nu a fost adaugat." << endl;
        return;
    }

    angajatNou.setIdAngajat(nextAngajatId++);
    angajati.push_back(angajatNou);
    setColorAdmin(10);
    cout << "\nAngajat adaugat cu succes! ID: " << angajatNou.getIdAngajat() << endl;
    salveazaDateAngajati();
}

void Administrator::afiseazaAngajati() const
{
    if (angajati.empty())
    {
        setColorAdmin(9);
        cout << "Nu exista angajati inregistrati." << endl;
        return;
    }
    setColorAdmin(14);
    cout << left
         << setw(6) << "ID" << setw(15) << "Nume" << setw(15) << "Prenume"
         << setw(15) << "CNP" << setw(25) << "Functie" << setw(18) << "Salariu(RON)"
         << setw(18) << "Data Ang." <<endl;
    cout << string(105, '-') << endl;

    for (const auto& angajat : angajati)
    {
        setColorAdmin(8);
        cout << left
             << setw(6) << angajat.getIdAngajat()
             << setw(15) << angajat.getNume()
             << setw(15) << angajat.getPrenume()
             << setw(15) << angajat.getCNP()
             << setw(25) << angajat.getFunctie()
             << fixed << setprecision(2) << setw(18) << angajat.getSalariu()
             << setw(12) << angajat.getDataAngajare().toString() << endl;
    }
}


double Administrator::getIncasariPerioada(const Data& start, const Data& end) const
{
    double totalIncasari = 0.0;

    for (const auto& rez : hotel->getRezervari())
    {
        if (rez.getStare() == StareRezervare::CheckIn || rez.getStare() == StareRezervare::CheckOut)
        {
            if (!(rez.getCheckOut() < start) && (rez.getCheckOut() < end || rez.getCheckOut().toString() == end.toString()) )
            {
                totalIncasari += rez.getPretTotal();
            }
        }
    }
    return totalIncasari;
}

Data avanseazaZi(Data d)
{

    int zileInLunaVal = Data::getZileInLuna(d.luna, d.an);

    d.zi++;
    if (d.luna > 0 && d.luna <= 12 && d.zi > zileInLunaVal && zileInLunaVal != 0)
    {
        d.zi = 1;
        d.luna++;
        if (d.luna > 12)
        {
            d.luna = 1;
            d.an++;
        }
    }
    else if (d.luna <= 0 || d.luna > 12 || zileInLunaVal == 0)
    {
        return d;
    }
    return d;
}


double Administrator::getGradOcuparePerioada(const Data& start, const Data& end) const
{
    if (!start.esteValida() || !end.esteValida() || end < start) return 0.0;

    long long totalNoptiDisponibile = 0;
    long long noptiOcupate = 0;
    int numarTotalCamere = hotel->getCamere().size();

    Data currentDate = start;

    while (!(end < currentDate))
    {
        if (!currentDate.esteValida()) break;
        totalNoptiDisponibile += numarTotalCamere;

        for (const auto& rez : hotel->getRezervari())
        {
            if (rez.getStare() != StareRezervare::Anulata && rez.getStare() != StareRezervare::InAsteptare)
            {
                if ((rez.getCheckIn() < currentDate || rez.getCheckIn().toString() == currentDate.toString()) &&
                        (currentDate < rez.getCheckOut()))
                {
                    noptiOcupate++;
                }
            }
        }
        if (currentDate.toString() == end.toString()) break;
        currentDate = avanseazaZi(currentDate);
        if (!currentDate.esteValida() || (currentDate.toString() == start.toString() && !(start.toString() == end.toString())))
        {
            setColorAdmin(12);
            cerr << "\nEroare in avansarea datei in getGradOcuparePerioada. Intrerupere bucla." << endl;
            break;
        }
    }

    if (totalNoptiDisponibile == 0) return 0.0;
    return ((double)noptiOcupate / totalNoptiDisponibile) * 100.0;
}


void Administrator::schimbaCredentiale()
{
    string currentPassInput;
    string newUsernameInput;
    string newPassInput;
    string newPassConfirmInput;

    setColorAdmin(9);
    cout << "\n--- Schimbare Credentiale Administrator ---" << endl;

    setColorAdmin(8);
    cout << "Introduceti parola curenta: ";
    setColorAdmin(14);
    cin >> currentPassInput;

    if (criptareDecriptare(currentPassInput, XOR_KEY) != this->encryptedPassword)
    {
        setColorAdmin(12);
        cout << "\nParola curenta este incorecta. Schimbarea credentialelor a fost anulata." << endl;
        return;
    }

    setColorAdmin(8);
    cout << "\nIntroduceti noul username: ";
    setColorAdmin(14);
    cin >> newUsernameInput;

    setColorAdmin(8);
    cout << "Introduceti noua parola: ";
    setColorAdmin(14);
    cin >> newPassInput;

    if (newPassInput.length() < 6)
    {
        setColorAdmin(12);
        cout << "\nNoua parola trebuie sa aiba cel putin 6 caractere. Schimbarea credentialelor a fost anulata." << endl;
        return;
    }

    setColorAdmin(8);
    cout << "Confirmati noua parola: ";
    setColorAdmin(14);
    cin >> newPassConfirmInput;

    if (newPassInput != newPassConfirmInput)
    {
        setColorAdmin(12);
        cout << "\nParolele noi nu se potrivesc. Schimbarea credentialelor a fost anulata." << endl;
        return;
    }

    salveazaCredentialeNoi(newUsernameInput, newPassInput);
    setColorAdmin(10);
    cout << "\nUsername-ul si parola au fost schimbate cu succes." << endl;
}


void Administrator::vizualizeazaIncasari() const
{
    Data dataStart, dataSfarsit;
    Data dataCurenta = Data::getDataCurenta();
    const string numeLuni[] = {"", "Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie",
                               "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"
                              };



    system("cls");
    setColorAdmin(14);
    cout << "\nIncasari pentru anul " << dataCurenta.getAn() << ":\n" << endl;

    double incasariTotale = 0.0;
    for (int luna = 1; luna <= 12; ++luna)
    {
        double incasariLuna = 0.0;
        for (const auto& rez : hotel->getRezervari())
        {
            if ((rez.getStare() == StareRezervare::CheckIn || rez.getStare() == StareRezervare::CheckOut) &&
                    rez.getCheckIn().getLuna() == luna &&
                    rez.getCheckIn().getAn() == dataCurenta.getAn())
            {
                incasariLuna += rez.getPretTotal();
            }
        }
        incasariTotale += incasariLuna;
        setColorAdmin(8);
        cout << left << setw(12) << numeLuni[luna] << "- " << fixed << setprecision(2) << incasariLuna << " RON" << endl;
    }

    setColorAdmin(3);
    cout << "\nTotal - " << fixed << setprecision(2) << incasariTotale << " RON" << endl;


}


void Administrator::statisticiGradOcupare() const
{
    if (hotel->getCamere().empty())
    {
        cout << "\nNu exista camere inregistrate pentru a calcula gradul de ocupare." << endl;
        return;
    }

    Data dataStart, dataSfarsit;
    Data dataCurenta = Data::getDataCurenta();
    const string numeLuni[] = {"", "Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie",
                               "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"
                              };



    system("cls");
    setColorAdmin(14);
    cout << "\nGrad de ocupare pentru anul " << dataCurenta.getAn() << ":\n" << endl;

    // Calcul grad ocupare pentru fiecare lună
    for (int luna = 1; luna <= 12; ++luna)
    {
        dataStart = Data(1, luna, dataCurenta.getAn());
        int zileLuna = Data::getZileInLuna(luna, dataCurenta.getAn());
        if (zileLuna == 0)
        {
            setColorAdmin(12);
            cout << "\nEroare: Luna " << luna << " invalidă." << endl;
            continue;
        }
        dataSfarsit = Data(zileLuna, luna, dataCurenta.getAn());

        if (!dataStart.esteValida() || !dataSfarsit.esteValida())
        {
            setColorAdmin(12);
            cout << "\nEroare: Date invalide pentru luna " << numeLuni[luna] << "." << endl;
            continue;
        }

        double gradOcupare = getGradOcuparePerioada(dataStart, dataSfarsit);
        setColorAdmin(8);
        cout << left << setw(12) << numeLuni[luna] << "- " << fixed << setprecision(2) << gradOcupare << "%" << endl;
    }

    // Calcul grad ocupare total pe an
    dataStart = Data(1, 1, dataCurenta.getAn());
    dataSfarsit = Data(31, 12, dataCurenta.getAn());
    if (!dataStart.esteValida() || !dataSfarsit.esteValida())
    {
        setColorAdmin(12);
        cout << "\nEroare: Date invalide pentru an." << endl;
        return;
    }
    double gradOcupareAnual = getGradOcuparePerioada(dataStart, dataSfarsit);
    setColorAdmin(3);
    cout << "\nTotal - " << fixed << setprecision(2) << gradOcupareAnual << "%" << endl;
}

