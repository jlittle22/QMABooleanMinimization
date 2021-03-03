#ifndef MINIMIZER_H
#define MINIMIZER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <inttypes.h>
#include <algorithm>

#include "minterm.h"

using namespace std;

class Minimizer {
  public:
  	// constructor 
    Minimizer(string in);

    // destructor
    ~Minimizer();

    // minimizes the truth table and returns equation
    string minimize();

    // prints truth table (duh?)
    void printTruthTable();

    // prints minterm groups (^)
    void printMintermGroups();

    // prints the essential primes table (ok, that one's harder)
    void printEPITable();

  private:
  	// string passed to minimizer representing outcomes of truth table
  	string inputString;

  	// indices of the minterms in inputString
  	vector<int> minterms;

  	// the number of variables the system is operator on
  	unsigned numVars = 3;

  	// the truth table
  	vector<vector<char>> table;

  	// maps number of ones to minterms
  	unordered_map<int, vector<Minterm>> mintermGroups;

    // essential primes table
  	unordered_map<Minterm, unordered_map<int, bool>, Minterm::hash_fn> essentialPrimesTable;

    // pairs close minterms in the mintermGroups with one another to reduce mintermGroups
  	bool pairCloseMinterms();

    // initial grouping of minterms with same number of 1's
  	void groupMinterms();

  	// converts the minterm at table[*][m] to binary
  	string mintermToBinary(unsigned m);

    // initializes the EPI with true and false values
  	void initializeEPITable();

  	// finds the essential primes and returns the set
  	unordered_set<Minterm, Minterm::hash_fn> findEPIs();
};

#endif