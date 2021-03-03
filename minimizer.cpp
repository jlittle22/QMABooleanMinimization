#include "minimizer.h"

#include <iostream>

using namespace std;

// utility function returns whether n is a power of two
static bool isPowerOfTwo(int n) {
	if (n == 0) return false;
	if (ceil(log2(n)) == floor(log2(n))) return true;
	else return false;
}

// utility function to calculate the size of the runs in
// the variables columns of the truth table
static int calculateRunSize(int x, int numVars) {
    int size = 1;
    for (int i = 0; i < numVars - 1 - x; i++) {
        size = size << 1;
    }
    return size;
}

// utility function to count number of 1's in binary rep of n
static int calculateNumberOfOnes(int n) {
    int count = 0;
    for (int i = 0; i < 32; i++) {
        count += n & 1;
        n = n >> 1;
    }
    return count;
}

// utility function to count 1's in string b
static int countOnesInBinaryString(string b) {
    int count = 0;
    for (unsigned i = 0; i < b.length(); i++) {
        if (b[i] == '1') {
            count++;
        }
    }
    return count;
}

// checks if two binary strings are off by exactly one character
static bool isOffByOne(string b1, string b2) {
    if (b1.length() != b2.length()) {
        throw runtime_error("minterm binary representations are different lengths");
    }
    bool mismatch = false;
    for (unsigned i = 0; i < b1.length(); i++) {
        if (b1[i] != b2[i]) {
            if (mismatch) {
                return false;
            } else {
                mismatch = true;
            }
        }
    }
    if (!mismatch) {
        throw runtime_error("minterm binary representations match");
    }
    return true;
}

// utility function to combine two binary representations where
// b1 and b2 are off by exactly 1 character
static string combineBinaryRepresentations(string b1, string b2) {
    if (b1.length() != b2.length()) {
        throw runtime_error("minterm binary representations are different lengths");
    }
    string res = string(b1.length(), '0');
    unsigned count = 0;
    for (unsigned i = 0; i < b1.length(); i++) {
        // if equal, great - insert it
        if (b1[i] == b2[i]) {
            res[i] = b1[i];
        // if unequal, insert '-'
        } else {
            res[i] = '-';
            count++;
        }
    }
    
    // can't be off by more than 1 or 0 so this is bed
    if (count != 1) {
        throw runtime_error("comining minterm binaries resulted in not 1 differences");
    }

    return res;
}

Minimizer::Minimizer(string in) {
    // validate length of input
    const unsigned n = in.length();
    if (!isPowerOfTwo(n)) {
        throw runtime_error("Your input string's length is not a power of 2.");
    }

    // set private data members
    inputString = in;
    numVars = log2(n);

    // initialize truth table variables section
    for (unsigned i = 0; i < numVars; i++) {
        vector<char> col;
        int run_size = calculateRunSize(i, numVars);
        for (unsigned j = 0; j < n; j++) {
            char c = (j / run_size) % 2 ? '1' : '0'; 
            col.push_back(c);
        }
        table.push_back(col);
    }

    // initialize truth table result section
    vector<char> results;
    for (unsigned i = 0; i < n; i++) {
        if (in[i] == '1' || in[i] == 'x' || in[i] == 'X') {
            minterms.push_back(i);
        }
        results.push_back(in[i]);
    }
    table.push_back(results);
}

Minimizer::~Minimizer() {

}

// utility function to check if a string contains all dashes
static bool isAllDashes(string in) {
    for (char& c : in) {
        if (c != '-') return false;
    }
    return true;
}

// minimizes the inputString to a logic equation and returns
string Minimizer::minimize() {
    printTruthTable();
	// group minterms together by number of 1s
    groupMinterms();
    printMintermGroups();

    // while we can still combine minterms, do so
    while(pairCloseMinterms()) { printMintermGroups(); }

    // initialize essential prime implicant table 
    initializeEPITable();

    // find a set of essential prime implicants
    unordered_set<Minterm, Minterm::hash_fn> essentialPrimes = findEPIs();

    // if there's only one essential prime ... 
    if (essentialPrimes.size() == 1) {
        // if it's all dashes, the input was all 1's
        if (isAllDashes(essentialPrimes.begin()->getAsBinary())) {
            return "1";
        // else the input was just a single PI
        } else {
            return essentialPrimes.begin()->getAsLogic();
        }
    // if we have no essential primes, we had no minterms so the equation is 0
    } else if (essentialPrimes.size() == 0) {
        return "0";
    // otherwise sum all the PIs and send it out
    } else {
        string equation = "";
        for (const auto& m : essentialPrimes) {
            equation = equation + "+"  + m.getAsLogic();
        } 
        return equation.substr(1); 
    }

}
    
// converts the minterm at table[*][m] to binary
string Minimizer::mintermToBinary(unsigned m) {
    if (m < 0 || m >= inputString.length()) {
        throw runtime_error("failed to retrieve binary representation of mintern.");
    }
    string res = "";
    for (unsigned i = 0; i < numVars; i++) {
        res += table[i][m];
    }
    return res;
}

// initial grouping of minterms with same number of 1's
void Minimizer::groupMinterms() {
    int n = inputString.length();
    for (int i = 0; i < n; i++) {
        // if current input char is a 1 or Dont Care ... 
        if (inputString[i] == '1' || inputString[i] == 'X' || inputString[i] == 'x') {
            // count the number of 1s
            int num_ones = calculateNumberOfOnes(i);
            // init a new minterm
            Minterm insert = Minterm(i, inputString[i] == 'x' || inputString[i] == 'X', mintermToBinary(i));
            // insert it into its group
            if (mintermGroups.find(num_ones) == mintermGroups.end()) {
                vector<Minterm> v{ insert };
                mintermGroups[num_ones] = v;
            } else {
                mintermGroups[num_ones].push_back(insert);
            }
        }
    }
}

// pairs close minterms in the mintermGroups with one another to reduce mintermGroups
bool Minimizer::pairCloseMinterms() {
    unordered_map<int, vector<Minterm>> newMintermGroups;
    unsigned totalMatches = 0;
    for (unsigned i = 0; i < numVars; i++) {
        if (mintermGroups.find(i) == mintermGroups.end()) {
            continue;
        }
        int next = i + 1;
        if (mintermGroups.find(next) == mintermGroups.end()) {
            while (mintermGroups.find(next) == mintermGroups.end() && (unsigned)next < numVars - 1) {
                next++;
            }
        }
        for (unsigned j = 0; j < mintermGroups[i].size(); j++) {
            for (unsigned k = 0; k < mintermGroups[next].size(); k++) {
                string b1 = mintermGroups[i][j].getAsBinary();
                string b2 = mintermGroups[next][k].getAsBinary();
                if (isOffByOne(b1, b2)) {
                    totalMatches++;
                    mintermGroups[i][j].setMatched(true);
                    mintermGroups[next][k].setMatched(true);
                    string newBinary = combineBinaryRepresentations(b1, b2);
                    Minterm insert = Minterm(mintermGroups[i][j],
                                                         mintermGroups[next][k],
                                                         newBinary
                                                        );
                    int numOnes = countOnesInBinaryString(newBinary);
                    if (newMintermGroups.find(numOnes) == newMintermGroups.end()) {
                        vector<Minterm> v { insert };
                        newMintermGroups[numOnes] = v;
                    } else {
                        if (find(newMintermGroups[numOnes].begin(), newMintermGroups[numOnes].end(), insert)
                            == newMintermGroups[numOnes].end()) {
                            newMintermGroups[numOnes].push_back(insert);
                        }
                    }
                }
            }
        }
    }

    // make sure we bring along the unmatched minterms from this round to the next
    // also, if we didn't match anything, we're done.
    if (totalMatches == 0) {
        return false;
    } else {
        for (auto& it : mintermGroups) {
            for (Minterm& m : it.second) {
                if (!m.isMatched()) {
                    m.setMatched(false);
                    if (newMintermGroups.find(it.first) == newMintermGroups.end()) {
                        vector<Minterm> v { m };
                        newMintermGroups[it.first] = v;
                    } else {
                        newMintermGroups[it.first].push_back(m);
                    }
                }
            }
        }
        mintermGroups = newMintermGroups;
        return true;
    }
}

// prints ...
void Minimizer::printMintermGroups() {
    for(const auto& it : mintermGroups) {
        if (it.second.begin() == it.second.end()) continue;
        cout << "Group ";
        cout << it.first << endl; 
        for_each(it.second.begin(), it.second.end(), [] (const Minterm& m_i) {
            cout << "   Minterm(s) Value(s):";
            for (const int& i : m_i.getMintermValues()) {
                cout << i << " ";
            }
            cout << endl;
            cout << "   Is IDC: " << m_i.isDontCare() << endl;
            cout << "   " << m_i.getAsBinary() << endl;
        });
    }
}

// prints ...
void Minimizer::printTruthTable() {
    cout << "Truth Table (" + to_string(numVars) + " variables)" << endl;
    for (unsigned i = 0; i < table[0].size(); i++) {
        for (unsigned j = 0; j < table.size() - 1; j++) {
            cout << table[j][i] << " ";
        }
        cout << "| " << table[table.size() - 1][i] << endl;
    }
}

// prints ...
void Minimizer::printEPITable() {
    cout << "--- EPI Table ---" << endl;
    for (const auto& m : essentialPrimesTable) {
        cout << "Minterm containing: ";
        for (const auto& i : m.first.getMintermValues()) {
            cout << i << " ";
        }
        cout << "==> ";
        for (const auto& v : m.second) {
            cout << "( " << v.first << ", " << v.second << " ) ";
        }
        cout << endl;
    }
}

// intializes the EPI table 
void Minimizer::initializeEPITable() {
    for(const auto& it : mintermGroups) {
        for(const auto& m : it.second) {
            for (const auto& v : minterms) {
                essentialPrimesTable[m][v] = m.contains(v);
            }
        }
    }
}

// utility function to check if set candidate satisfies all minterms in missing
static bool isSolutionToUnsatified(unordered_set<Minterm, Minterm::hash_fn> candidate, unordered_set<int> missing) {
    for (const auto& m : candidate) {
        for (auto it = missing.begin(); it != missing.end();) {
            if (m.contains(*it)) {
                it = missing.erase(it);
            } else {
                ++it;
            }
        }
    }
    return missing.empty();
}

// finds the essential prime implicants
unordered_set<Minterm, Minterm::hash_fn> Minimizer::findEPIs() {
    unordered_set<Minterm, Minterm::hash_fn> essentials;
    for (const auto& mintermKey : minterms) {
        int countChecks = 0;
        const Minterm* checked;
        for (const auto& m : essentialPrimesTable) {
            if (essentialPrimesTable[m.first][mintermKey] == true) {
                checked = &m.first;
                countChecks++;
            }
        }
        // if an original minterm is missing, add it to missed.
        if (countChecks == 0) {
            // @ME: if a Dont Care value is missing, shouldn't it be ignored? ... probably.
            bool dc = inputString[mintermKey] == 'x' || inputString[mintermKey] == 'X';
            Minterm missed = Minterm(mintermKey, dc, mintermToBinary(mintermKey));
            if (essentials.find(missed) == essentials.end()) {
                essentials.insert(missed);
            }
        // if there's exactly one paired minterm covering this original, the paired is essential
        } else if (countChecks == 1) {
            if (essentials.find(*checked) == essentials.end()) {
                essentials.insert(*checked);
            }
        }
    }

    // determine what original minterms are not covered by our solution despite
    // being covered by SOME paired minterm(s)
    unordered_set<int> missedOriginals;
    for (const auto& orig : minterms) {
        bool found = false;
        for (const auto& m : essentials) {
            if (m.contains(orig)) {
                found = true;
            }
        }
        if (!found) {
            missedOriginals.insert(orig);
        }
    }
  
    // if we've missing something...
    if (!missedOriginals.empty()) {
        
        // STEP 1: get vector of NON-essential minterms
        vector<Minterm> nonEssentialMinterms;
        for (const auto& it : essentialPrimesTable) {
            if (essentials.find(it.first) == essentials.end()) {
                nonEssentialMinterms.push_back(it.first);
            }
        }

        // check all combinations of non-essential minterms and keep track of
        // how many terms each combination takes
        unsigned minSize = nonEssentialMinterms.size() + 1;
        unordered_set<Minterm, Minterm::hash_fn> bestSol;

        unsigned long long powerSetSize = (unsigned long long)1 << nonEssentialMinterms.size();
        for (unsigned long long i = 0; i < powerSetSize; i++) {
            unordered_set<Minterm, Minterm::hash_fn> candidate;
            for (unsigned long long j = 0; j < nonEssentialMinterms.size(); j++) {
                unsigned long long mask = (unsigned long long)1 << j;
                if (i & mask) {
                    candidate.insert(nonEssentialMinterms[j]);
                }
            }
            if (isSolutionToUnsatified(candidate, missedOriginals)) {
                unsigned size = candidate.size();
                if (size < minSize || bestSol.empty()) {
                    bestSol = candidate;
                    minSize = size;  
                }
            }
        }
        // select the solution that took the least number of minterms
        // and add it to essentials
        if (bestSol.empty()) {
            throw runtime_error("No solution for some missed originals. This is a bug. Or you exhausted a datatype's finite memory. :)");
        }
        for (auto& m : bestSol) {
            essentials.insert(m);
        }
    }

    return essentials;
}


