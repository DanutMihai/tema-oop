#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
// Definirea clasei Carte
using json = nlohmann::json;
class Carte {
private:
	std::string titlu;
	std::string autor;
	std::string domeniu;
	std::string cota;
	bool esteImprumutata;
	std::chrono::system_clock::time_point dataImprumut;
	int zileImprumut;
public:
	Carte(const std::string& tit, const std::string& aut, const std::string& dom, std::string cot)
		: titlu(tit), autor(aut), domeniu(dom), cota(cot), esteImprumutata(false), zileImprumut(0) {}

	void imprumuta(int zile) {
		esteImprumutata = true;
		dataImprumut = std::chrono::system_clock::now();
		zileImprumut = zile;
	}
	void setZileImprumut(int zile) {
		zileImprumut = zile;
	}
	void returneaza() {
		esteImprumutata = false;
	}

	bool verificaCartiTermenDepasit() const {
		if (esteImprumutata) {
			auto acum = std::chrono::system_clock::now();
			auto dueDate = dataImprumut + std::chrono::hours(24 * zileImprumut);
			return acum > dueDate;
		}
		return false;
	}

	// Getteri si alte metode necesare
	std::string getTitlu() const { return titlu; }
	std::string getAutor() const { return autor; }
	std::string getDomeniu() const { return domeniu; }
	std::string getCota() const { return cota; }
	bool getEsteImprumutata() const { return esteImprumutata; }
	int getZileImprumut() const { return zileImprumut; }
};

// Definirea clasei Biblioteca
class Biblioteca {
private:
	std::vector<Carte> carti;
	std::map<std::string, std::vector<Carte>> domenii;

public:

	// Metoda pentru crearea unui nou domeniu
	void creazaDomeniu(const std::string& numeDomeniu) {
		if (domenii.find(numeDomeniu) == domenii.end()) {
			domenii[numeDomeniu] = std::vector<Carte>();
			std::cout << "Domeniul '" << numeDomeniu << "' a fost creat.\n";
		}
		else {
			std::cout << "Domeniul '" << numeDomeniu << "' exista deja.\n";
		}
	}

	void stergeCarte(std::string cota) {
		auto it = std::remove_if(carti.begin(), carti.end(), [cota](const Carte& carte) { return carte.getCota() == cota; });
		if (it != carti.end()) {
			carti.erase(it, carti.end());
			std::cout << "Cartea a fost stearsa cu succes";
		}
		else {
			std::cout << "Cartea cu cota " << cota << " nu a fost gasita.\n";
		}
	}


	// Metoda pentru imprumutarea unei carti
	void imprumutaCarte(const std::string& cota) {
		for (auto& carte : carti) {
			if (carte.getCota() == cota && !carte.getEsteImprumutata()) {
				carte.imprumuta(carte.getZileImprumut());
				updateBookInDomenii(carte); // Update the book in domenii map
				std::cout << "\nCartea cu cota " << cota << " a fost imprumutata.\n";
				return;
			}
		}
		std::cout << "Cartea nu este disponibila sau nu a fost gasita.\n";
	}
	void returneazaCarte(const std::string& cota) {
		for (auto& carte : carti) {
			if (carte.getCota() == cota && carte.getEsteImprumutata()) {
				carte.returneaza();
				updateBookInDomenii(carte); // Update the book in domenii map
				std::cout << "Cartea cu cota " << cota << " a fost returnata.\n";
				return;
			}
		}
		std::cout << "Cartea nu este imprumutata sau nu a fost gasita.\n";
	}
	// New helper method to update the book in the domenii map
	void updateBookInDomenii(const Carte& updatedBook) {
		auto& booksInDomain = domenii[updatedBook.getDomeniu()];
		auto it = std::find_if(booksInDomain.begin(), booksInDomain.end(),
			[&updatedBook](const Carte& carte) { return carte.getCota() == updatedBook.getCota(); });
		if (it != booksInDomain.end()) {
			*it = updatedBook;
		}
	}

	// Metoda pentru verificarea cartilor cu termen depasit de imprumut
	void verificaCartiTermenDepasit() {
		for (const auto& domeniu : domenii) {
			for (const auto& carte : domeniu.second) {
				if (carte.getEsteImprumutata() && carte.verificaCartiTermenDepasit()) {
					std::cout << "Cartea " << carte.getTitlu() << " este cu termenul depasit.\n";
				}
			}
		}
	}
	// Metoda pentru adaugarea unei noi carti

	void adaugaCarte() {
		std::string titlu, autor, domeniu, cota;
		std::cout << "Titlu: ";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // curatare buffer input
		std::getline(std::cin, titlu); // getline pentru a permite spatii in titlu
		std::cout << "Autor: ";
		std::getline(std::cin, autor);
		std::cout << "Domeniu: ";
		std::getline(std::cin, domeniu);
		std::cout << "Cota: ";
		std::getline(std::cin, cota);

		Carte newBook(titlu, autor, domeniu, cota);
		if (domenii.find(domeniu) == domenii.end()) {
			domenii[domeniu] = std::vector<Carte>(); // Creaza domeniul daca nu exista 
		}
		domenii[domeniu].push_back(newBook); // Adauga cartea la acel domeniu
		carti.push_back(newBook); // Adauga cartea in vectorul cu toate cartile
	}


	void cautaSiAfiseazaCarte() {
		std::string searchTerm;
		std::cout << "Introduceti titlul sau autorul cartii pentru cautare: ";
		std::cin.ignore(); // curatare  caracter newline din stream
		std::getline(std::cin, searchTerm);

		for (const auto& domeniu : domenii) {
			for (const auto& carte : domeniu.second) {
				if (carte.getTitlu() == searchTerm || carte.getAutor() == searchTerm) {
					std::cout << "Cartea a fost gasita in domeniul " << domeniu.first << ": "
						<< carte.getTitlu() << " de " << carte.getAutor() << "\n";
					return;
				}
			}
		}
		std::cout << "Cartea nu a fost gasita.\n";
	}

	void afiseazaToateCartile() {
		std::cout << "Lista completa a cartilor din biblioteca:\n";
		for (const auto& carte : carti) {
			std::cout << "Titlu: " << carte.getTitlu()
				<< ", Autor: " << carte.getAutor()
				<< ", Domeniu: " << carte.getDomeniu()
				<< ", Cota: " << carte.getCota()
				<< ", Imprumutata: " << (carte.getEsteImprumutata() ? "Da" : "Nu")
				<< "\n" << std::endl;
		}
	}

	// Metoda pentru incarcarea cartilor dintr-un fisier
	void incarcaCartiDinJson(const std::string& fisierNume) {
		std::ifstream fisier(fisierNume);
		if (!fisier.is_open()) {
			std::cerr << "Nu se poate deschide fisierul JSON." << std::endl;
			return;
		}

		nlohmann::json j;
		try {
			fisier >> j;
		}
		catch (nlohmann::json::parse_error& e) {
			std::cerr << "Eroare la parsarea JSON: " << e.what() << std::endl;
			return;
		}

		// Verifics daca structura JSON este asa cum te aștepti
		if (!j.contains("biblioteca") || !j["biblioteca"].contains("carti")) {
			std::cerr << "Structura JSON nu este corectă." << std::endl;
			return;
		}

		for (const auto& item : j["biblioteca"]["carti"]) {
			// Asigura-te ca fiecare camp necesar exista si este tipul corect
			if (!item.contains("titlu") || !item["titlu"].is_string() ||
				!item.contains("autor") || !item["autor"].is_string() ||
				!item.contains("domeniu") || !item["domeniu"].is_string() ||
				!item.contains("cota") || !item["cota"].is_string() ||
				!item.contains("imprumutata") || !item["imprumutata"].is_string()) {
				std::cerr << "Lipsesc câmpuri într-una dintre cărți sau tipurile de date nu sunt corecte." << std::endl;
				continue; // Sari peste aceasta carte si continua cu urmatoarea
			}

			std::string titlu = item["titlu"];
			std::string autor = item["autor"];
			std::string domeniu = item["domeniu"];
			std::string cota = item["cota"];
			bool esteImprumutata = (item["imprumutata"] == "Da");

			Carte carte(titlu, autor, domeniu, cota);
			if (esteImprumutata) {
				carte.imprumuta(carte.getZileImprumut());
			}
			carti.push_back(carte);
			domenii[domeniu].push_back(carte);
		}
	}

	Carte* TestSearch(const std::string& cota) {
		for (auto& carte : carti) {
			if (carte.getCota() == cota) {
				return &carte; // Return the address of the found Carte object
			}
		}
		return nullptr; // Return nullptr if no matching Carte is found
	}



	Carte* cautaCarte(const std::string& cautare, bool dupaTitlu = true) {
		for (auto& carte : carti) {
			if ((dupaTitlu && carte.getTitlu() == cautare) || (!dupaTitlu && carte.getAutor() == cautare)) {
				return &carte;
			}
		}
		return nullptr;
	}
};
void afiseazaMeniu() {
	std::cout << "\n1. Incarcarea datelor despre cartile din biblioteca din fisierul carti.in\n";
	std::cout << "2. Crearea unui nou domeniu.\n";
	std::cout << "3. Adaugarea unei carti la un anumit domeniu.\n";
	std::cout << "4. Cautarea unei carti in biblioteca dupa titlu sau autor.\n";
	std::cout << "5. Stergerea unei carti din biblioteca.\n";
	std::cout << "6. Imprumutare / Returnare carte (dupa cota).\n";
	std::cout << "7. Verificare carti cu termen depasit de imprumut.\n";
	std::cout << "9. Iesire\n";
	std::cout << "\nAlegeti o optiune si apasati enter :  ";
}

int main() {
	Biblioteca biblioteca;
	int optiune;
	std::string numeDomeniu;
	biblioteca.incarcaCartiDinJson("carti.json");  //incarcare la inceput 

	do {
		afiseazaMeniu();

		while (!(std::cin >> optiune)) {
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			std::cout << "\nOptiune invalida. Incercati din nou.\n\n";
			std::cin.clear(); // Reseteaza starea stream-ului
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			afiseazaMeniu();
		}


		switch (optiune) {
		case 1: {
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			biblioteca.afiseazaToateCartile();
			break;
		}
		case 2:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character left in the buffer
			std::cout << "Introduceti numele noului domeniu: ";
			std::getline(std::cin, numeDomeniu); // Get the entire line, including spaces
			biblioteca.creazaDomeniu(numeDomeniu);
			break;

		case 3:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			biblioteca.adaugaCarte();
			break;

		case 4:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			biblioteca.cautaSiAfiseazaCarte();
			break;

		case 5: {
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			std::string cota;
			std::cout << "Introduceti cota cartii pentru stergere: ";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Curata buffer
			std::getline(std::cin, cota);
			biblioteca.stergeCarte(cota);
			break;
		}
		case 6: {
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			std::string cota;
			std::cout << "Introduceti cota cartii pentru imprumut/returnare: ";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::getline(std::cin, cota);
			Carte* carte = biblioteca.TestSearch(cota);
			if (carte != nullptr) {
				if (carte->getEsteImprumutata()) {
					biblioteca.returneazaCarte(cota);
				}
				else {
					biblioteca.imprumutaCarte(cota);
				}
			}
			else {
				std::cout << "Cartea cu cota " << cota << " nu a fost gasita.\n";
			}
			break;
		}
		case 7:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			biblioteca.verificaCartiTermenDepasit();
			break;
		case 8:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			biblioteca.incarcaCartiDinJson("carti.json");
			std::cout << "Datele despre carti au fost reincarcate." << std::endl;
			break;
		case 9:
			std::cout << "La revedere!\n";
			break;
		default:
			std::cout << "\033[2J\033[1;1H"; // Curata ecranul si muta cursorul la inceput
			std::cout << "\nOptiune invalida. Incercati din nou.\n\n";
			afiseazaMeniu();
			break;
		}
	} while (optiune != 9);

	return 0;

}
