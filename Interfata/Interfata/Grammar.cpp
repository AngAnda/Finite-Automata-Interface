#include "Grammar.h"

Grammar::Grammar(const std::vector<char>& neterminale, const std::vector<char>& terminale, const char& startSymbol, const std::vector<std::pair<std::vector<char>, std::vector<char>>>& productie, const char& lambda)
	:m_neterminale(neterminale),
	m_terminale(terminale),
	m_startSymbol(startSymbol),
	m_productie(productie)
{};

Grammar::Grammar()
{};

bool Grammar::verificaNeterminale() {
	return !m_neterminale.empty();
}

bool Grammar::verificaTerminale() {
	return !m_terminale.empty();
}

bool Grammar::verificaSimbolStart() {
	return std::find(m_neterminale.begin(), m_neterminale.end(), m_startSymbol) != m_neterminale.end() &&
		std::find(m_terminale.begin(), m_terminale.end(), m_startSymbol) == m_terminale.end();
}

bool Grammar::verificaElementeComune() {
	std::set<char> neterminaleSet{ m_neterminale.begin(), m_neterminale.end() };
	for (const auto& terminal : m_terminale) {
		if (neterminaleSet.find(terminal) != neterminaleSet.end()) {
			return false;
		}
	}
	return true;
}

bool Grammar::verificaProductii() {
	bool startSymbolInProductie = false;
	for (const auto& prod : m_productie) {
		bool containsNeterminal = false;
		for (auto& caracter : prod.first) {
			if (std::find(m_neterminale.begin(), m_neterminale.end(), caracter) != m_neterminale.end()) {
				containsNeterminal = true;
				break;
			}
		}
		if (!containsNeterminal) {
			return false;
		}
		if (prod.first[0] == m_startSymbol) {
			startSymbolInProductie = true;
		}
		for (auto& caracter : prod.second) {
			if (std::find(m_neterminale.begin(), m_neterminale.end(), caracter) == m_neterminale.end() &&
				std::find(m_terminale.begin(), m_terminale.end(), caracter) == m_terminale.end() && caracter != m_lambda) {
				return false;
			}
		}
	}
	return startSymbolInProductie;
}

bool Grammar::existaProductieLambda()
{
	for (const auto& prod : m_productie) {
		if (prod.first[0] == m_startSymbol && prod.first.size() == 1 && prod.second[0] == m_lambda && prod.second.size() == 1) {
			return true;
		}
	}
	return false;
}

bool Grammar::verificare()
{
	if (!verificaNeterminale()) {
		std::cout << "Trebuie sa existe cel putin un simbol neterminal\n";
		return false;
	}
	if (!verificaTerminale()) {
		std::cout << "Multimea terminalelor nu poate fi vida\n";
		return false;
	}
	if (!verificaSimbolStart()) {
		std::cout << "Simbolul de start nu este valid, introduceti altul\n";
		return false;
	}
	if (!verificaElementeComune()) {
		std::cout << "Multimea de neterminale si terminale nu trebuie sa aiba elemente comune\n";
		return false;
	}
	if (!verificaProductii()) {
		std::cout << "Productii invalide\n";
		return false;
	}
	return true;
}

bool Grammar::isIDC()
{
	bool IDC = true;
	if (verificare() == false)
		std::cout << "Gramatica nu este valida\n";
	else {
		for (const auto& prod : m_productie) {
			if (prod.first.size() != 1)
				return false;
			else {
				if (std::find(m_neterminale.begin(), m_neterminale.end(), prod.first[0]) == m_neterminale.end())
					return false;
			}
		}
	}
	return true;
}

std::vector<char> Grammar::generare()
{
	std::vector<char> curent = { m_startSymbol };
	bool existaNeterminale = true;

	while (existaNeterminale)
	{
		for (auto& c : curent)
			std::cout << c;
		std::cout << " => ";

		existaNeterminale = false;
		std::vector<std::pair<int, std::pair<std::vector<char>, std::vector<char>>>> posibilitati;

		for (size_t i = 0; i < curent.size(); ++i)
		{
			for (auto& prod : m_productie)
			{
				if (prod.first.size() == 1 && prod.first[0] == curent[i])
				{
					posibilitati.emplace_back(i, prod);
					existaNeterminale = true;
				}
			}
		}

		if (posibilitati.empty())
		{
			break;
		}

		int randomIndex = random_generator(posibilitati.size());
		auto& inlocuire = posibilitati[randomIndex];

		if (inlocuire.second.second.empty() || inlocuire.second.second[0] == m_lambda)
		{
			curent.erase(curent.begin() + inlocuire.first);
		}
		else
		{
			curent.erase(curent.begin() + inlocuire.first);
			curent.insert(curent.begin() + inlocuire.first, inlocuire.second.second.begin(), inlocuire.second.second.end());
		}
	}

	if (curent.empty())
	{
		curent.push_back(m_lambda);
	}

	std::cout << "Cuvant final: ";
	for (auto& c : curent)
		std::cout << c;
	std::cout << "\n";

	return curent;
}


int Grammar::random_generator(int size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, size - 1);

	int random = distr(gen);
	return random;
}

std::istream& operator>>(std::istream& in, Grammar& grammar) {
	std::string line;

	if (std::getline(in, line)) {
		for (char c : line) {
			if (std::isalnum(c)) {
				grammar.m_neterminale.push_back(c);
			}
		}
	}

	if (std::getline(in, line)) {
		for (char c : line) {
			if (std::isalnum(c)) {
				grammar.m_terminale.push_back(c);
			}
		}
	}

	if (std::getline(in, line) && std::isalnum(line[0])) {
		grammar.m_startSymbol = line[0];
	}

	while (std::getline(in, line)) {
		std::regex regex("([A-Za-z]+)\\s*=>\\s*([A-Za-z]*)");
		std::smatch match;
		if (std::regex_search(line, match, regex)) {
			std::vector<char> leftSide(match[1].first, match[1].second);
			std::vector<char> rightSide(match[2].first, match[2].second);

			grammar.m_productie.push_back(std::make_pair(leftSide, rightSide));
		}
	}

	return in;
}

std::ostream& operator<<(std::ostream& out, const Grammar& grammar)
{
	out << "Neterminale: ";
	for (int i = 0; i < grammar.m_neterminale.size(); ++i)
		out << grammar.m_neterminale[i] << " ";
	out << '\n';
	out << "Terminale: ";
	for (int i = 0; i < grammar.m_terminale.size(); ++i)
		out << grammar.m_terminale[i] << " ";
	out << '\n';
	out << "Simbol de start: " << grammar.m_startSymbol << '\n';
	out << "Productii: \n";
	for (std::vector<std::pair<std::vector<char>, std::vector<char>>>::const_iterator it = grammar.m_productie.begin(); it != grammar.m_productie.end(); ++it)
	{
		for (std::vector<char>::const_iterator it2 = it->first.begin(); it2 != it->first.end(); ++it2)
			out << *it2;
		out << " => ";
		if (it->second.empty())
			out << "lambda";
		for (std::vector<char>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			out << *it2;
		out << '\n';
	}
	return out;
}
