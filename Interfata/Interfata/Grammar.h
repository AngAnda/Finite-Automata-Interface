#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <random>
#include <algorithm>
#include <set>
#include <regex>

class Grammar
{
public:
	Grammar(const std::vector<char>& neterminale, const std::vector<char>& terminale, const char& startSymbol, const std::vector<std::pair<std::vector<char>, std::vector<char>>>& producti, const char& lambda = '*');
	Grammar();

	bool verificare();
	bool isIDC();
	std::vector<char> generare();
	//de facut simplificarea
	friend std::istream& operator>>(std::istream& in, Grammar& grammar);
	friend std::ostream& operator<<(std::ostream& out, const Grammar& grammar);

private:
	std::vector<char> m_neterminale;
	std::vector<char> m_terminale;
	char m_startSymbol;
	std::vector<std::pair<std::vector<char>, std::vector<char>>> m_productie;
	char m_lambda = '*';

	int random_generator(int size);

	bool verificaNeterminale();
	bool verificaTerminale();
	bool verificaSimbolStart();
	bool verificaElementeComune();
	bool verificaProductii();
	bool existaProductieLambda();
};

